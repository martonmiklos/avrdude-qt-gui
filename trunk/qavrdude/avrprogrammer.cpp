#include "avrprogrammer.h"
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QString>
AvrProgrammer::AvrProgrammer(Settings *sa, AvrPart *part, QObject *parent)
    : QObject(parent), currentPart(part), settings(sa), isTerminalMode(false)
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
}

void AvrProgrammer::readSignature()
{
    currentDudeTask = DudeTaskReadSignature;
    QString startString = staticProgrammerCommand();
    startString.append(" -F -U signature:r:"+signatureFile->fileName()+":h");
    signatureFile->open(QFile::WriteOnly);
    signatureFile->seek(0);
    signatureFile->close();
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

void AvrProgrammer::readyReadDudeOutPut()
{
    QByteArray outPut = avrDudeProcess->readAllStandardOutput();
    qWarning() << outPut << "\n";
    emit avrDudeOut(outPut);
    if ((currentDudeTask == DudeTaskReadFlash)      ||
            (currentDudeTask == DudeTaskWriteFlash)     ||
            (currentDudeTask == DudeTaskVerifyFlash)    ||
            (currentDudeTask == DudeTaskReadEEPROM)     ||
            (currentDudeTask == DudeTaskWriteEEPROM)    ||
            (currentDudeTask == DudeTaskVerifyEEPROM)) {
        if ((outPut.size() == 1) && (outPut[0] == '#')) {
            emit progressStep();
        }
    }

    if ((currentDudeTask == DudeTaskVerifyFlash) ||
            (currentDudeTask == DudeTaskVerifyEEPROM)) {
        if (outPut.contains("first mismatch at byte")) {
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
            if (currentDudeTask == DudeTaskVerifyEEPROM)
                emit verifyMismatch("EEPROM", offset, readVal, fileVal);
            else if (currentDudeTask == DudeTaskVerifyFlash)
                emit verifyMismatch("flash", offset, readVal, fileVal);
        }
    }

    if (isTerminalMode) {
        switch (currentDudeTask) {
        case DudeTaskSetVoltages:
            if (outPut.startsWith("avrdude>")) {
                if (currentDudeTask == DudeTaskSetVoltages) {
                    QString cmd;
                    switch (setVoltageStepCnt) {
                    case 3:
                        cmd = QString("vtarg %1\n").arg(vTarget);
                        break;
                    case 2:
                        cmd = QString("varef 0 %1\n").arg(aref0);
                        break;
                    case 1:
                        cmd = QString("varef 0 %1\n").arg(aref0);
                        break;
                    case 0:
                        cmd = "quit\n";
                        break;
                    }
                    avrDudeProcess->write(cmd.toAscii());
                    setVoltageStepCnt--;
                }
            }
            break;
        case DudeTaskGetVoltages:
            if (outPut.startsWith("avrdude>")) {
                if (getValueCMDSent) {
                    avrDudeProcess->write("quit\n");

                } else {
                    avrDudeProcess->write("parms\n");
                    getValueCMDSent = true;
                }
            } else if (getValueCMDSent) {

            }
            break;
        default:
            qWarning() << "we cannot get here it is a bug";
            break;
        }

    }
}

void AvrProgrammer::processErrorSlot(QProcess::ProcessError error)
{
    qWarning() << tr("Process error:") << error;
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
            emit taskFailed(QString(tr("Flash reading failed: avrdude retcode: %1.\n"
                                       "Check the <a href=\"tab_dudeout\">AVRDude output tab</a> for details")).arg(retcode));
        }
        break;
    case DudeTaskWriteFlash:
        if (!retcode) {
            emit taskFinishedOk("Programming the flash memory was successful.");
        } else {
            emit taskFailed(QString(tr("Flash writing failed: avrdude retcode: %1.<br>"
                                       "Check the <a href=\"tab_dudeout\">AVRDude output tab</a> for details")).arg(retcode));
        }
        break;
    case DudeTaskVerifyFlash:
        if (!retcode) {
            emit taskFinishedOk("Verification of the flash memory was successful.");
        } else {
            emit taskFailed(QString(tr("Verifying the flash memory failed: avrdude retcode: %1.\n"
                                       "Check the <a href=\"tab_dudeout\">AVRDude output tab </a> for details")).arg(retcode));
        }
        break;
    case DudeTaskReadFuse: {
        QString fuseValues;
        if (!retcode) {
            bool ok = true;
            for (int i = 0; i<fusesToRead.size(); i++)  { // loop trough the Avrpart'
                bool ok = false;
                QFile currentFuseOutFile(QDir::tempPath()+"/"+fusesToRead[i]+".txt");
                currentFuseOutFile.open(QFile::ReadOnly);
                quint8 currentFuseValue = currentFuseOutFile.readAll().trimmed().toInt(&ok, 16);
                currentFuseOutFile.close();
                if (ok == false) {
                    emit taskFailed(QString(tr("Unable to read the %1 fuse")).arg(fuseNamesToRead[i]));
                } else {
                    // fancy output to the status listbox
                    fuseValues.append(fusesToRead[i].toUpper()+
                                      ": 0x"+QString::number(currentFuseValue, 16).rightJustified(2, '0').toUpper());
                    if (i != (fusesToRead.size() -1))
                        fuseValues.append(", ");

                    for(int j = 0; j< currentPart->fuseRegs.size(); j++) {
                        if (getAvrDudeFuseNameFromXMLName(currentPart->fuseRegs.at(j)->name()) == fusesToRead[i]) {
                            currentPart->fuseRegs[j]->setValue(currentFuseValue);
                            currentPart->fusesChanged();
                            break;
                        }
                    }
                }
            }

            if (ok) {
                emit taskFinishedOk(QString(tr("Reading the fuse bits done (%1)")).arg(fuseValues));
                emit fusesReaded();
            }
        } else {
            emit taskFailed(tr("Failed to read the fuse bits"));
        }
    } break;
    case DudeTaskWriteFuse: {
        if (!retcode) {
            emit taskFinishedOk(tr("Writing the fuse bits was successful"));
        } else {
            emit taskFailed(tr("Failed to write the fuse bits"));
        }
    } break;
    case DudeTaskVerifyFuse: {
        if (!retcode) {
            emit taskFinishedOk("The setted fuse values matched the fuses in the device");
        } else {
            emit taskFailed(QString(tr("Fuse bits mismatch in the device.\n"
                                       "Check the <a href=\"tab_dudeout\">AVRDude output tab </a> for details")));
        }
    } break;
    case DudeTaskReadEEPROM: {
        if (!retcode) {
            emit taskFinishedOk("Reading the EEPROM memory was successful.");
        } else {
            emit taskFailed(QString(tr("EEPROM reading failed: avrdude retcode: %1.\n"
                                       "Check the <a href=\"tab_dudeout\">AVRDude output tab</a> for details")).arg(retcode));
        }
    } break;
    case DudeTaskVerifyEEPROM: {
        if (!retcode) {
            emit taskFinishedOk(tr("Verification of the EEPROM memory was successful."));
        } else {
            emit taskFailed(QString(tr("EEPROM verification failed: avrdude retcode: %1.\n"
                                       "Check the <a href=\"tab_dudeout\"> AVRDude output tab </a> for details\n").arg(retcode)));
        }
    } break;
    case DudeTaskWriteEEPROM: {
        if (!retcode) {
            emit taskFinishedOk("Programming the EEPROM memory was successful.");
        } else {
            emit taskFailed(QString(tr("EEPROM writing failed: avrdude retcode: %1.<br>"
                                       "Check the <a href=\"tab_dudeout\">AVRDude output tab</a> for details")).arg(retcode));
        }
    } break;
    case DudeTaskReadLock: {
        if (!retcode) {
            QFile currentLockByteFile(QDir::tempPath()+"/lockbyte_value.txt");
            if (currentLockByteFile.open(QFile::ReadOnly)) {
                bool ok = false;
                quint8 lockbyte = currentLockByteFile.readAll().trimmed().toInt(&ok, 16);
                currentLockByteFile.close();
                if (ok == false) {
                    emit taskFailed(tr("The lockbyte value is not hexadecimal in the %1 file")
                                    .arg(currentLockByteFile.fileName()));
                } else {
                    currentPart->lockBytes.first()->setValue(lockbyte); // FIXME if we found AVR part with multiple lockbytes
                    emit taskFinishedOk(tr("Reading the lockbyte was successful (0x%1)")
                                        .arg(QString::number(lockbyte, 16).rightJustified(2, '0')));
                    emit lockBitReaded();
                }
            } else {
                emit taskFailed(tr("Unable to open the %1 file").arg(currentLockByteFile.fileName()));
            }
        } else {
            emit taskFailed(tr("Failed to read the lock byte"));
        }
    } break;
    case DudeTaskSetVoltages: {
        if (!retcode) {
            emit taskFinishedOk(tr("Programming voltages had been set successfully."));
        } else {
            emit taskFailed(QString(tr("Unable to set programmer voltages: avrdude returned with error-code: %1.\n"
                                       "Check the <a href=\"tab_dudeout\"> AVRDude output tab </a> for details\n").arg(retcode)));
        }
    } break;
    case DudeTaskGetVoltages: {
        if (!retcode) {
            emit taskFinishedOk(tr("Programming voltages had been readed successfully."));
        } else {
            emit taskFailed(QString(tr("Unable to read programmer voltages: avrdude returned with error-code: %1.\n"
                                       "Check the <a href=\"tab_dudeout\"> AVRDude output tab </a> for details\n").arg(retcode)));
        }
    } break;
    case DudeTaskNone: {

    } break;
    default:
        qWarning() << "TODO IMPLEMENT IT";
        break;
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

void AvrProgrammer::programFlash(QString fileName, bool verifyAfter, bool eraseBefore)
{
    currentDudeTask = DudeTaskWriteFlash;
    QString startString = staticProgrammerCommand();
    QChar flashType = 'i';
    if (fileName.toLower().endsWith(".s")) {
        flashType = 's';
    } else if (fileName.toLower().endsWith(".bin") || fileName.toLower().endsWith(".raw")) {
        flashType = 'r';
    }

    startString.append(" -U flash:w:"+fileName+":"+flashType);
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

void AvrProgrammer::programEEPROM(QString hexFileName)
{
    currentDudeTask = DudeTaskWriteEEPROM;
    QString startString = staticProgrammerCommand();
    startString.append(" -U eeprom:w:"+hexFileName+":i");
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

void AvrProgrammer::verifyEEPROM(QString hexFileName)
{
    currentDudeTask = DudeTaskVerifyEEPROM;
    QString startString = staticProgrammerCommand();
    startString.append(" -U eeprom:v:"+hexFileName+":i");
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

void AvrProgrammer::readEEPROM(QString hexFileName)
{
    currentDudeTask = DudeTaskReadEEPROM;
    QString startString = staticProgrammerCommand();
    startString.append(" -U eeprom:r:"+hexFileName+":i");
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

void AvrProgrammer::programFuses()
{
    currentDudeTask = DudeTaskWriteFuse;
    QString startString = staticProgrammerCommand();
    for (int i = 0; i<currentPart->fuseRegs.count(); i++) {
        QString currentFuseArg = getAvrDudeFuseNameFromXMLName(currentPart->fuseRegs[i]->name());
        startString.append(" -U "+currentFuseArg+":w:"
                           "0x"+QString::number(currentPart->fuseRegs[i]->value(), 16).rightJustified(2, '0')+":m");
    }
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

void AvrProgrammer::readFuses(QStringList fuseList)
{
    currentDudeTask = DudeTaskReadFuse;
    QString startString = staticProgrammerCommand();
    fusesToRead.clear();
    fuseNamesToRead = fuseList;
    for (int i = 0; i<fuseList.size(); i++)  {
        QString currentFuseArg = getAvrDudeFuseNameFromXMLName(fuseList[i]);
        fusesToRead.append(currentFuseArg);
        QFile currentFuseOutFile(QDir::tempPath()+"/"+currentFuseArg+".txt");
        if (currentFuseOutFile.exists())
            currentFuseOutFile.remove();
        startString.append(" -U "+currentFuseArg+":r:"+currentFuseOutFile.fileName()+":h");
    }
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

void AvrProgrammer::verifyFuses()
{
    currentDudeTask = DudeTaskVerifyFuse;
    QString startString = staticProgrammerCommand();
    fusesToRead.clear();
    for (int i = 0; i<currentPart->fuseRegs.size(); i++)  {
        QString currentFuseArg = getAvrDudeFuseNameFromXMLName(currentPart->fuseRegs[i]->name());
        fusesToRead.append(currentFuseArg);
        startString.append(" -U "+currentFuseArg+":v:0x"+QString::number(currentPart->fuseRegs.at(i)->value(), 16).rightJustified(2, '0')+":m");
    }
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

void AvrProgrammer::programLockByte()
{
    currentDudeTask = DudeTaskReadLock;
    QString startString = staticProgrammerCommand();
    startString.append(" -U  lock:w:0x"+QString::number(currentPart->lockBytes.first()->value(), 16).rightJustified(2,'0')+":m");
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

void AvrProgrammer::readLockByte()
{
    currentDudeTask = DudeTaskReadLock;
    QString startString = staticProgrammerCommand();
    QFile currentLockByteFile(QDir::tempPath()+"/lockbyte_value.txt");
    if (currentLockByteFile.exists())
        currentLockByteFile.remove();
    startString.append(" -U  lock:r:"+currentLockByteFile.fileName()+":h");
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}

QString AvrProgrammer::staticProgrammerCommand()
{
    QString ret;
    if (!settings->particularProgOptions) {
        ret = QString("%1 -p %2 -c %3 -P %4")
                .arg(settings->dudePath)
                .arg(settings->partName)
                .arg(settings->programmerName)
                .arg(settings->programmerPort);
    } else {
        ret = QString("%1 -p %3 %2")
                .arg(settings->dudePath)
                .arg(settings->programmerOptions)
                .arg(settings->partName);
    }
    return ret;
}


// returns the fuse name in the avrdude's argument format.
QString AvrProgrammer::getAvrDudeFuseNameFromXMLName(QString fuseName)
{
    if (fuseName.toLower() == "high")
        return "hfuse";

    if (fuseName.toLower() == "low")
        return "lfuse";

    if (fuseName.toLower() == "extended")
        return "efuse";

    // atxmega devices fuse fields are called
    // FUSE[N] in the xml file an the avrdude need the similar argument in lower case
    return fuseName.toLower();
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
    return -1;
}


void AvrProgrammer::setVoltagesSTK500(double vtarget_a, double aref0_a, double aref1_a)
{
    vTarget = vtarget_a;
    aref0 = aref0_a;
    aref1 = aref1_a;
    isTerminalMode = true;
    currentDudeTask = DudeTaskSetVoltages;
    QString startString = staticProgrammerCommand();
    startString.append(" -t");
    avrDudeProcess->start(startString);
    setVoltageStepCnt = 3;
    emit avrDudeOut(startString);
}

void AvrProgrammer::getVoltagesSTK500()
{
    isTerminalMode = true;
    currentDudeTask = DudeTaskGetVoltages;
    QString startString = staticProgrammerCommand();
    startString.append(" -t");
    getValueCMDSent = false;
    avrDudeProcess->start(startString);
    emit avrDudeOut(startString);
}
