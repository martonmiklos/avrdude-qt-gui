#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
class Settings : public QObject
{
Q_OBJECT
public:
    enum DeviceDataType {
        DeviceDb_XML = 0,
        DeviceDb_SQLite
    };

    explicit Settings(QObject *parent = 0);
    ~Settings();
    QString dudePath;
    QString xmlsPath, sqlitePath;
    bool showAvrDudeOutPut;
    bool particularProgOptions;
    QString programmerOptions;
    QString programmerName;
    QString programmerPort;
    QString partName;
    QString flashHexFile;
    QString eepromHexFile;
    bool rememberLastTab;
    int defaultTabIndex;
    int lastTabIndex;
    bool noicons;
    DeviceDataType deviceData;
private:
    QSettings *settingsIni;
signals:

public slots:

};

#endif // SETTINGS_H
