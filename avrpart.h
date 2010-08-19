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
    FuseModel *fuseModel;
    QStringList supportedFuses;

public:
    AvrPart(Settings *sa, FuseModel *fa, QObject *parent = 0);
    AvrPart(Settings *sa, FuseModel *fa, QString name, QObject *parent = 0);
    QString getPartName() const {return partNameStr;}
    bool setPartName(QString pn);
    bool init(QString pn);
    QString getSignature() const;
    QString error() const {return errorString;}
    QString getAvrDudePartNo(QString name) const;
    quint8 sign0, sign1, sign2; // signature bytes
    QString findDeviceWithSignature(quint8 s0, quint8 s1, quint8 s2);
    QStringList getSupportedFuses() const {return supportedFuses;}

signals:
    void reloadFuseView();

public slots:

};
#include "dudepartnos.h"


#endif // AVRPART_H
