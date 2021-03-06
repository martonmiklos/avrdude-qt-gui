#include "settings.h"

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    settingsIni = new QSettings("MM", "QAvrdude", this);
    settingsIni->beginGroup("misc");
    partName = settingsIni->value("partName", "m8").toString();
    showAvrDudeOutPut = settingsIni->value("showAvrDudeOutPut", false).toBool();
    deviceData = (DeviceDataType)settingsIni->value("deviceData", DeviceDb_SQLite).toInt();
    settingsIni->endGroup();

    settingsIni->beginGroup("paths");
    #ifdef Q_OS_LINUX
    dudePath = settingsIni->value("dudePath", "/usr/bin/avrdude").toString();
    xmlsPath = settingsIni->value("xmlsPath", "/usr/share/avrxmls").toString(); // FIXME check avrstudio default installation path
    sqlitePath = settingsIni->value("sqlitePath", "/usr/share/qavrdude/devicedb.sqlite").toString();
    #elif defined(Q_OS_WIN32)
    dudePath = settingsIni->value("dudePath", "C:\\winavr\\bin\\avrdude").toString();
    xmlsPath = settingsIni->value("xmlsPath", "").toString(); // FIXME check avrstudio default installation path
    sqlitePath = settingsIni->value("sqlitePath", "/usr/share/qavrdude/devicedb.sqlite").toString();
    #else
    dudePath = settingsIni->value("dudePath", "").toString(); // FIXMEE
    xmlsPath = settingsIni->value("xmlsPath", "/usr/share/avrxmls").toString(); // FIXME check avrstudio default installation path
    sqlitePath = settingsIni->value("sqlitePath", "/usr/share/qavrdude/devicedb.sqlite").toString();
    #endif

    flashHexFile = settingsIni->value("flashHexFile").toString();
    eepromHexFile = settingsIni->value("eepromHexFile").toString();
    settingsIni->endGroup();

    settingsIni->beginGroup("gui");
    rememberLastTab = settingsIni->value("rememberLastTab", true).toBool();
    defaultTabIndex = settingsIni->value("defaultTabIndex", 0).toInt();
    lastTabIndex = settingsIni->value("lastTabIndex", 0).toInt();
    noicons = settingsIni->value("noicons", false).toBool();
    settingsIni->endGroup();

    settingsIni->beginGroup("programmerOptions");
    particularProgOptions = settingsIni->value("particularProgOptions", false).toBool();
    programmerOptions = settingsIni->value("programmerOptions", "-c stk500 -P /dev/ttyUSB0").toString();
    programmerPort = settingsIni->value("programmerPort", "/dev/ttyUSB0").toString();
    programmerName = settingsIni->value("programmerName", "stk500").toString();
    settingsIni->endGroup();
}

Settings::~Settings()
{
    settingsIni->beginGroup("misc");
    settingsIni->setValue("partName", partName);
    settingsIni->setValue("showAvrDudeOutPut", showAvrDudeOutPut);
    settingsIni->setValue("deviceData", deviceData);
    settingsIni->endGroup();

    settingsIni->beginGroup("paths");
    settingsIni->setValue("dudePath", dudePath);
    settingsIni->setValue("xmlsPath", xmlsPath);
    settingsIni->setValue("sqlitePath", sqlitePath);
    settingsIni->setValue("flashHexFile", flashHexFile);
    settingsIni->setValue("eepromHexFile", eepromHexFile);
    settingsIni->endGroup();

    settingsIni->beginGroup("gui");
    settingsIni->setValue("rememberLastTab", rememberLastTab);
    settingsIni->setValue("defaultTabIndex", defaultTabIndex);
    settingsIni->setValue("lastTabIndex", lastTabIndex);
    settingsIni->setValue("noicons", noicons);
    settingsIni->endGroup();

    settingsIni->beginGroup("programmerOptions");
    settingsIni->setValue("particularProgOptions", particularProgOptions);
    settingsIni->setValue("programmerOptions", programmerOptions);
    settingsIni->setValue("programmerName", programmerName);
    settingsIni->setValue("programmerPort", programmerPort);
    settingsIni->endGroup();
    settingsIni->sync();
}
