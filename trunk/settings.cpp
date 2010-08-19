#include "settings.h"

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    settingsIni = new QSettings("MM", "QAvrdude", this);
    settingsIni->beginGroup("misc");
    partName = settingsIni->value("partName", "m8").toString();
    showAvrDudeOutPut = settingsIni->value("showAvrDudeOutPut", false).toBool();
    settingsIni->endGroup();

    settingsIni->beginGroup("paths");
    #ifdef Q_OS_LINUX
    dudePath = settingsIni->value("dudePath", "/usr/bin/avrdude").toString();
    xmlsPath = settingsIni->value("xmlsPath", "/usr/share/avrxmls").toString(); // FIXME check avrstudio default installation path
    #elif defined(Q_OS_WIN32)
    dudePath = settingsIni->value("dudePath", "C:\winavr\bin\avrdude").toString(); // FIXMEE check winavr default path
    xmlsPath = settingsIni->value("xmlsPath", ""); // FIXME check avrstudio default installation path
    #else
    dudePath = settingsIni->value("dudePath", "").toString(); // FIXMEE
    xmlsPath = settingsIni->value("xmlsPath", "/usr/share/avrxmls").toString(); // FIXME check avrstudio default installation path
    #endif

    flashHexFile = settingsIni->value("flashHexFile").toString();
    eepromHexFile = settingsIni->value("eepromHexFile").toString();
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
    settingsIni->endGroup();

    settingsIni->beginGroup("paths");
    settingsIni->setValue("dudePath", dudePath);
    settingsIni->setValue("xmlsPath", xmlsPath);
    settingsIni->setValue("flashHexFile", flashHexFile);
    settingsIni->setValue("eepromHexFile", eepromHexFile);
    settingsIni->endGroup();

    settingsIni->beginGroup("programmerOptions");
    settingsIni->setValue("particularProgOptions", particularProgOptions);
    settingsIni->setValue("programmerOptions", programmerOptions);
    settingsIni->setValue("programmerName", programmerName);
    settingsIni->setValue("programmerPort", programmerPort);
    settingsIni->endGroup();
    settingsIni->sync();
}
