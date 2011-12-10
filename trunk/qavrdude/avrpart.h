#ifndef AVRPART_H
#define AVRPART_H

#include <QObject>
#include <QDomDocument>
#include <QDomElement>
#include <QtCore/QFile>
#include <QDebug>
#include <QStringList>
#include "settings.h"
#include "bitfieldmodel.h"

class AvrPart : public QObject
{
    Q_OBJECT
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

    // FIXME make PPP
    QList <Register*> fuseRegs;
    QList <Register*> lockBytes;

    RegistersModel* fusesModel() const {return m_fusesModel;}
    RegisterFieldsModel *fuseFieldsModel() const {return m_fuseFieldsModel;}

    RegistersModel* lockByteModel() const {return m_lockBytesModel;}
    RegisterFieldsModel *lockByteFieldsModel() const {return m_lockByteFieldsModel;}

    void fusesChanged();

private:
    // avr related variables
    QString partNameStr; // teh normal name of the controller (for e.g Atmega8)
    QString avrdudePartNo; // the partname in avrdude option stlyle representation (for e.g. m8 for Atmega8)

    QString errorString;

    // misc xml handling variables
    QDomDocument domDoc;
    QFile domFile;

    bool fillFuseAndLockData();
    bool findXml(QString);
    QMap<QString, QString> dudePartNos;
    Settings *settings;

    RegistersModel *m_fusesModel;
    RegisterFieldsModel *m_fuseFieldsModel;

    RegistersModel *m_lockBytesModel;
    RegisterFieldsModel *m_lockByteFieldsModel;
};

#endif // AVRPART_H
