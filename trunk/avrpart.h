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

class LockBitField
{
public:
    LockBitField(){}
    QString shortName, text;
    int mask;
    int value; // value is similar like in the XML file (right aligned)
    bool isEnum;
    QMap <int, QString> enumValues;// predefined group values & their name
};

Q_DECLARE_METATYPE(LockBitField);

class LockRegister
{
public:
    LockRegister(){}
    //LockRegister(QString name, int offset, int size) :name(name), offset(offset), size(size){}
    QList<LockBitField> bitFields;
    quint8 value;
    QString name;
    int offset;
    int size;
};

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
    bool fillLockBitModel();
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
    LockRegister lockbyte;
    QString getFuseRegisterBitName(QString fuseReg, int bitnum);
    QString getFuseRegisterBitName(int fuseReg, int bitnum);

signals:
    void reloadFuseView();
};

#endif // AVRPART_H
