#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
class Settings : public QObject
{
Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);
    ~Settings();
    QString dudePath;
    QString xmlsPath;
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
private:
    QSettings *settingsIni;
signals:

public slots:

};

#endif // SETTINGS_H
