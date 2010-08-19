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
    DudeTaskProgramFlash,
    DudeTaskReadFlash,
    DudeTaskVerifyFlash,
    DudeTaskReadFuse,
    DudeTaskWriteFuse,
    DudeTaskVerifyFuse,

} CurrentDudeTask;

class AvrProgrammer : public QObject
{
    Q_OBJECT
public:
    AvrProgrammer(Settings *sa, QObject *parent = 0);
    void setPart(AvrPart *part) {currentPart = part;}
    QString getFriendlyName() const {return friendlyName;}
    QString getDudeName() const {return avrDudeName;}
    PortType getPortType() const {return portType;}
    void readSignature();
    void eraseDevice();
    void programFlash(QString hexFileName, bool verifyAfter = true, bool eraseBefore = true);
    void verifyFlash(QString hexFileName);
    void readFlash(QString hexFileName);
    void readFuse();
    void writeFuse(quint8 hfuse, quint8 lfuse);
    bool isWorking() const {return currentDudeTask == DudeTaskNone;}

signals:
    void signatureReadSignal(quint8 sign0, quint8 sing1, quint8 sign2);
    void progressStep();
    void avrDudeOut(QString out);
    void taskFinishedOk(QString);
    void taskFailed(QString);
    void fuseRead(quint8 h, quint8 l);
    void verifyMismatch(QString what, int offset, int value_read, int value_file);

private:
    PortType portType;
    QString friendlyName;
    QString avrDudeName;
    AvrPart *currentPart;
    QProcess *avrDudeProcess;
    Settings *settings;
    QFile *signatureFile;
    QFile *hFuseFile, *lFuseFile;

    CurrentDudeTask currentDudeTask;
    QString staticProgrammerCommand();

    int getFirstHexNumberFromStr(QString str, bool & success, int & numberEnd);

private slots:
    void readyReadDudeOutPut();
    void processErrorSlot(QProcess::ProcessError error);
    void dudeFinished(int retcode);
};

#endif // AVRPROGRAMMER_H
