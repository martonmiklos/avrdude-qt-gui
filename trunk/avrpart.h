#ifndef AVRPART_H
#define AVRPART_H

#include <QObject>
#include <QDomDocument>
#include <QDomElement>
#include <QtCore/QFile>
#include <QDebug>
#include <QStringList>
#include "settings.h"
#include "fusemodel.h"

class FuseRegister;

class AvrPart : public QObject
{
    Q_OBJECT
private:
    // avr related variables
    QString partNameStr;
    QString avrdudePartNo;

    QString errorString;
    QDomDocument domDoc;
    QFile domFile;

    void fillPartNosMap(); // FIXME make it static
    bool fillFuseModel();
    bool findXml(QString);
    QMap<QString, QString> dudePartNos;
    Settings *settings;

public:
    AvrPart(Settings *sa, QString name, QObject *parent = 0);
    QString getPartName() const {return partNameStr;}
    bool setPartName(QString pn);
    QString getSignature() const;
    QString error() const {return errorString;}
    QString getAvrDudePartNo(QString name) const;
    quint8 sign0, sign1, sign2; // signature bytes
    QString findDeviceWithSignature(quint8 s0, quint8 s1, quint8 s2);
    QStringList getSupportedFuses();
    QList <FuseRegister> fuseRegs;
    QString getFuseRegisterBitName(QString fuseReg, int bitnum);
    QString getFuseRegisterBitName(int fuseReg, int bitnum);

signals:
    void reloadFuseView();

public slots:
    void fusesReaded(QMap<QString, quint8> fuseValues);

};

#endif // AVRPART_H
