#ifndef AVRPROGRAMMER_H
#define AVRPROGRAMMER_H
#include <QString>
#include <QObject>
#include <QProcess>

#include "avrpart.h"
#include "settings.h"

typedef enum
{
    PortTypeUnknown     =  0,
    PortTypeNotNeeded   =  1,
    PortTypeSpecial     =  2,
    PortTypeSerial      =  3,
    PortTypeParallel    =  4,
    PortTypeUSB         =  5,

} PortType;

typedef enum
{
    DudeTaskNone ,
    DudeTaskReadSignature,
    DudeTaskErase,
    DudeTaskWriteFlash,
    DudeTaskReadFlash,
    DudeTaskVerifyFlash,
    DudeTaskReadFuse,
    DudeTaskWriteFuse,
    DudeTaskVerifyFuse,
    DudeTaskReadEEPROM,
    DudeTaskWriteEEPROM,
    DudeTaskVerifyEEPROM,
    DudeTaskReadLock,
    DudeTaskWriteLock,
    DudeTaskVerifyLock,

} CurrentDudeTask;

class AvrProgrammer : public QObject
{
    Q_OBJECT
public:
    AvrProgrammer(Settings *sa, AvrPart *part, QObject *parent = 0);
    QString getFriendlyName() const {return friendlyName;}
    QString getDudeName() const {return avrDudeName;}
    PortType getPortType() const {return portType;}
    void readSignature();
    void eraseDevice();

    void programFlash(QString hexFileName, bool verifyAfter = true, bool eraseBefore = true);
    void verifyFlash(QString hexFileName);
    void readFlash(QString hexFileName);

    void programEEPROM(QString hexFileName);
    void verifyEEPROM(QString hexFileName);
    void readEEPROM(QString hexFileName);

    void programFuses();
    void readFuses(QStringList fuseList);
    void verifyFuses(QStringList fuseList);

    void programLockByte();
    void readLockByte();
    void verifyLockByte();

    bool isWorking() const {return currentDudeTask == DudeTaskNone;}

signals:
    void signatureReadSignal(quint8 sign0, quint8 sing1, quint8 sign2);
    void progressStep();
    void avrDudeOut(QString out);
    void taskFinishedOk(QString);
    void taskFailed(QString);
    void fusesReaded();
    void lockBitReaded();
    void verifyMismatch(QString what, int offset, int value_read, int value_file);

private:
    PortType portType;
    QString friendlyName;
    QString avrDudeName;
    AvrPart *currentPart;
    QProcess *avrDudeProcess;
    Settings *settings;
    QFile *signatureFile;
    QStringList fusesToRead, fuseNamesToRead;
    CurrentDudeTask currentDudeTask;
    QString staticProgrammerCommand();

    QString getAvrDudeFuseNameFromXMLName(QString fuseName);

    int getFirstHexNumberFromStr(QString str, bool & success, int & numberEnd);

private slots:
    void readyReadDudeOutPut();
    void processErrorSlot(QProcess::ProcessError error);
    void dudeFinished(int retcode);
};

#endif // AVRPROGRAMMER_H
