#include "avrprogrammer.h"
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QString>
AvrProgrammer::AvrProgrammer(Settings *sa, QObject *parent)
    : settings(sa), QObject(parent)
{
    if (friendlyName.isEmpty())
        friendlyName = avrDudeName;
    avrDudeProcess = new QProcess(this);
    avrDudeProcess->setProcessChannelMode(QProcess::MergedChannels);
    avrDudeProcess->setReadChannel(QProcess::StandardOutput);
    connect(avrDudeProcess,SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadDudeOutPut()));
    connect(avrDudeProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processErrorSlot(QProcess::ProcessError)));
    connect(avrDudeProcess, SIGNAL(finished(int)), this, SLOT(dudeFinished(int)));
    signatureFile = new QFile(QDir::tempPath()+"/signature.txt");
    hFuseFile = new QFile(QDir::tempPath()+"/hfuse.txt");
    lFuseFile = new QFile(QDir::tempPath()+"/lfuse.txt");
}

void AvrProgrammer::readSignature()
{
    currentDudeTask = DudeTaskReadSignature;
    QString startString = staticProgrammerCommand();
    startString.append(" -U signature:r:"+signatureFile->fileName()+":h");
    signatureFile->open(QFile::WriteOnly);
    signatureFile->seek(0);
    signatureFile->close();
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

void AvrProgrammer::readyReadDudeOutPut()
{
    QByteArray outPut = avrDudeProcess->readAllStandardOutput();
    emit avrDudeOut(outPut);
    if ((currentDudeTask == DudeTaskReadFlash) ||
        (currentDudeTask == DudeTaskProgramFlash) ||
        (currentDudeTask == DudeTaskVerifyFlash)) {
        if ((outPut.size() == 1) && (outPut[0] == '#')) {
            emit progressStep();
        }
    }

    if (currentDudeTask == DudeTaskVerifyFlash) {
        if (outPut.contains("first mismatch at byte")) {
            bool ok = false;
            int offset, readVal, fileVal;
            bool success = false;
            int numEnd = 0;
            offset = getFirstHexNumberFromStr(outPut, success, numEnd);
            if (!success)
                return;
            outPut = outPut.mid(numEnd);
            readVal = getFirstHexNumberFromStr(outPut, success, numEnd);
            if (!success)
                return;
            outPut = outPut.mid(numEnd);
            fileVal = getFirstHexNumberFromStr(outPut, success, numEnd);
            emit verifyMismatch("flash", offset, readVal, fileVal);
        }
    }
}

void AvrProgrammer::processErrorSlot(QProcess::ProcessError error)
{
    qWarning() << error;
}

void AvrProgrammer::dudeFinished(int retcode)
{
    switch(currentDudeTask) {
    case DudeTaskReadSignature: {
            signatureFile->open(QIODevice::ReadOnly);
            QString signature = signatureFile->readAll();
            if (signature.isEmpty()) {
                emit taskFailed(QString(tr("The %1/signature.txt file is empty")).arg(QDir::tempPath()));
                emit taskFailed(tr("Reading the signature failed"));
            } else {
                QStringList signatures;
                signatures = signature.split(',');
                quint8 sign0, sign1, sign2;
                if (signatures.size() < 3) {
                    emit taskFailed(tr("Unable to read all signature bytes"));
                }
                bool ok = false;
                sign0 = signatures[0].toInt(&ok,16);
                sign1 = signatures[1].toInt(&ok,16);
                sign2 = signatures[2].toInt(&ok,16);
                if (!ok) {
                    emit taskFailed(QString("Unable to convert the signature bytes.\n Check the contents of the %1/signature.txt file").arg(QDir::tempPath()));
                } else {
                    emit signatureReadSignal(sign0, sign1, sign2);
                }
            }
            signatureFile->close();
        } break;
    case DudeTaskErase: {
            if (retcode) {
                emit taskFailed(QString("Erasing failed\nThe avrdude process ended with %1 code").arg(retcode));
            } else {
                emit taskFinishedOk("Erasing done");
            }
        }
        break;
    case DudeTaskReadFlash:
        if (!retcode) {
            emit taskFinishedOk("Reading the program memory was successful.");
        } else {
            emit taskFinishedOk(QString(tr("Flash reading failed: avrdude retcode: %1.\n"
                                        "Check the AVRDude output tab for details")).arg(retcode));
        }
        break;
    case DudeTaskProgramFlash:
        if (!retcode) {
            emit taskFinishedOk("Programming the flash memory was successful.");
        } else {
            emit taskFinishedOk(QString(tr("Flash writing failed: avrdude retcode: %1.\n"
                                        "Check the AVRDude output tab for details")).arg(retcode));
        }
        break;
    case DudeTaskVerifyFlash:
        if (!retcode) {
            emit taskFinishedOk("Verification of the flash memory was successful.");
        } else {
            emit taskFinishedOk(QString(tr("Flash writing failed: avrdude retcode: %1.\n"
                                        "Check the AVRDude output tab for details")).arg(retcode));
        }
        break;
    case DudeTaskReadFuse: {
            if (!retcode) {
                bool ok = true;
                hFuseFile->open(QFile::ReadOnly);
                quint8 hFuse = hFuseFile->readAll().trimmed().toInt(&ok, 16);
                hFuseFile->close();
                if (ok == false)
                    emit taskFailed("Unable to read the High fuse");

                lFuseFile->open(QFile::ReadOnly);
                quint8 lFuse = lFuseFile->readAll().trimmed().toInt(&ok, 16);
                lFuseFile->close();
                if (ok == false)
                    emit taskFailed("Unable to read the Low fuse");

                qWarning() << "fuses: H:" << hFuse << "L:" << lFuse;

                if (ok) {
                    emit taskFinishedOk(QString(tr("Reading the fuse bits done")));
                    emit fuseRead(hFuse, lFuse);
                }
            } else {
                emit taskFailed("Failed to read the fuse bits");
            }
        } break;
    }
    currentDudeTask = DudeTaskNone;
}

void AvrProgrammer::eraseDevice()
{
    currentDudeTask = DudeTaskErase;
    QString startString = staticProgrammerCommand();
    startString.append(" -e");
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

void AvrProgrammer::programFlash(QString hexFileName, bool verifyAfter, bool eraseBefore)
{
    currentDudeTask = DudeTaskProgramFlash;
    QString startString = staticProgrammerCommand();
    startString.append(" -U flash:w:"+hexFileName+":i");
    if (!verifyAfter)
        startString.append(" -V");
    if (!eraseBefore)
        startString.append(" -D");
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

void AvrProgrammer::verifyFlash(QString hexFileName)
{
    currentDudeTask = DudeTaskVerifyFlash;
    QString startString = staticProgrammerCommand();
    startString.append(" -U flash:v:"+hexFileName+":i");
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

void AvrProgrammer::readFlash(QString hexFileName)
{
    currentDudeTask = DudeTaskReadFlash;
    QString startString = staticProgrammerCommand();
    startString.append(" -U flash:r:"+hexFileName+":i");
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

void AvrProgrammer::readFuse()
{
    currentDudeTask = DudeTaskReadFuse;
    hFuseFile->remove();
    lFuseFile->remove();
    QString startString = staticProgrammerCommand();
    startString.append(" -U hfuse:r:"+hFuseFile->fileName()+":h -U lfuse:r:"+lFuseFile->fileName()+":h");
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

QString AvrProgrammer::staticProgrammerCommand()
{
    QString ret;
    if (!settings->particularProgOptions) {
        ret = QString("%1 -F -p %2 -c %3 -P %4")
                              .arg(settings->dudePath)
                              .arg(settings->partName)
                              .arg(settings->programmerName)
                              .arg(settings->programmerPort);
    } else {
        ret = QString("%1 -F -p %3 %2")
                      .arg(settings->dudePath)
                      .arg(settings->programmerOptions)
                      .arg(settings->partName);
    }
    return ret;
}

int AvrProgrammer::getFirstHexNumberFromStr(QString str, bool &success, int &numberEnd)
{
    try {
        int numStart = str.indexOf("0x");
        if (numStart == -1) {
            throw false;
        }
        QString tmpStr = str.mid(numStart + 2);
        if (!tmpStr.isEmpty()) {
            int i = 0;
            QChar ch = tmpStr[i];
            QString numStr;
            while (ch.isLetterOrNumber()) {
                numStr.append(ch);
                i++;
                if (tmpStr.size() > i)
                    ch = tmpStr[i];
                else
                    break;
            }
            bool ok = false;
            int ret = numStr.toInt(&ok, 16);
            if (ok) {
                numberEnd = numStart + i;
                success = true;
                return ret;
            } else {
                throw false;
            }
        } else {
            throw false;
        }
    } catch(bool ok) {
        if (ok == false) {
            success = false;
            numberEnd = -1;
            return -1;
        }
    }
}

