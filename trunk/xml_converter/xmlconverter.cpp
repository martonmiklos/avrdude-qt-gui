#include "xmlconverter.h"
#include <QDir>
#include <QDebug>
#include <QVariant>
#include <QMessageBox>
#include <QSqlError>

XMLConverter::XMLConverter(QObject *parent) :
    QObject(parent)
{
    database = QSqlDatabase::addDatabase("QSQLITE");
}

bool XMLConverter::convert(QString sqlite, QString xmlsDir)
{
    QDir dir(xmlsDir);
    QStringList fileList = dir.entryList(QStringList("*.xml"));

    database.setDatabaseName(sqlite);
    if (!database.open()) {
        logMessage(tr("Unable to open database"));
        return false;
    }

    QStringList tables;
    tables << "bitfields" << "bitfields_enums" << "device_register_types" << "devices" << "devices_registers";

    foreach (QString table , tables) {
        QSqlQuery query(database);
        query.prepare("DELETE FROM " + table);
        if (!query.exec()) {
            logMessage(tr("Unable to truncate table: %1").arg(table));
            return false;
        }
    }

    /*if (!parseFile(dir.path()+"/ATmega128.xml")) {
        emit logMessage(tr("Parsing failed"));
        return false;
    }
    return true;*/

    foreach (QString devxml, fileList) {
        if (!parseFile(dir.path()+"/"+devxml)) {
            emit logMessage(tr("Parsing failed"));
            return false;
        }
    }

    return true;
}


void XMLConverter::sqlError(QSqlQuery *query)
{
    QMessageBox::critical(NULL,
                          tr("SQL error"),
                          tr("An error happend during executing the following query:\n"
                             "%1\n"
                             "Error string:\n"
                             "%2")
                          .arg(query->lastQuery())
                          .arg(query->lastError().text()));
}

bool XMLConverter::parseFile(QString file)
{
    startTime = QTime::currentTime();
    QSqlQuery query(database);
    domFile.setFileName(file);
    qWarning() << domFile.fileName();
    QFileInfo info(domFile);

    query.prepare("INSERT INTO devices (name) VALUES(:device)");
    query.bindValue(":device", info.baseName());
    if (!query.exec()) {
        sqlError(&query);
        return false;
    }

    int deviceId = 0;
    query.exec("SELECT last_insert_rowid()");
    if (query.next()) {
        deviceId = query.value(0).toInt();
    } else {
        logMessage(tr("Cannot retrive last_insert_rowid()"));
        return false;
    }

    if (!domFile.open(QFile::ReadOnly))  {
        throw QString(tr("Could not open the %1 xml file")).arg(file);
    }

    if (!domDoc.setContent(&domFile)) {
        domFile.close();
        return false;
    }
    domFile.close();

    QDomElement v2 = domDoc.documentElement().firstChildElement("V2");
    printTimeElapsed();
    if (v2.isNull()) {
        qWarning() << tr("Could not find the V2 tag in the xml file. Maybe you should update it to a newer one.");
        return false;
    } else {
        QDomNodeList list = v2.elementsByTagName("registers");
        int regCount = 0;
        for (int i = 0; i<list.size() ;i++){
            if ((list.item(i).toElement().attribute("memspace") == "FUSE") ||
                (list.item(i).toElement().attribute("memspace") == "LOCKBIT")) {
                printTimeElapsed();
                qWarning() << i;
                parseRegisterDetailsFromNode(list.item(i), deviceId, query);
                regCount++;
                if (regCount == 2)
                    break; // quit if both register types had been found
            }
        }
    }
    emit logMessage(tr("Parsing taken %1 ms").arg(startTime.msecsTo(QTime::currentTime())));
    return true;
}

/*
 * Pass the tags like: <registers name="FUSE" memspace="FUSE">
 */
void XMLConverter::parseRegisterDetailsFromNode(QDomNode registersNode, int deviceId, QSqlQuery & query)
{
    // this will iterate on the high, low, extended fuses or on fuse[N] keys at xmega devices
    for(int i = 0; i<registersNode.childNodes().count(); i++) {
        QDomElement regElement = registersNode.childNodes().item(i).toElement();
        if (regElement.attribute("name") == "")
            continue; // ignore the offset only regs

        // first check that is not there te same type register in the database
        int registerTypeId = 0, registerId = 0;
        qWarning() << regElement.attribute("name");
        query.prepare("SELECT ROWID FROM device_register_types WHERE name = :registername");
        query.bindValue(":registername", regElement.attribute("name"));
        if (!query.exec()) {
            sqlError(&query);
        }

        // if found store the ID in the registerTypeId
        if (query.next()) {
            registerTypeId = query.value(0).toInt();
        } else {
            // if not found insert a new type
            query.prepare("INSERT INTO device_register_types (name) VALUES(:name)");
            query.bindValue(":name", regElement.attribute("name"));
            if (!query.exec())
                sqlError(&query);

            if (query.exec("SELECT last_insert_rowid()")) {
                query.next();
                registerTypeId = query.value(0).toInt();
            } else {
                sqlError(&query);
            }
        }

        // add a new register to the device
        query.prepare("INSERT INTO devices_registers (device_id, type_id) VALUES (:device_id, :type_id)");
        query.bindValue(":device_id", deviceId);
        query.bindValue(":type_id", registerTypeId);

        if (!query.exec()) {
            sqlError(&query);
        }


        if (!query.exec("SELECT last_insert_rowid()"))
            sqlError(&query);
        else {
            if (query.next())
                registerId = query.value(0).toInt();
        }

        // add all bitmasks to the register
        qWarning() << "regchildcount" << regElement.childNodes().count();
        for(int j = 0; j<regElement.childNodes().count(); j++) { // loop trough the current register's bitfields
            QDomElement bitFieldElement = regElement.childNodes().item(j).toElement();
            if (bitFieldElement.tagName() != "bitfield")
                continue;

            int bitFieldId = -1;
            query.prepare("INSERT INTO bitfields (name, short_name, mask, register_id)"
                          " VALUES (:name, :short_name, :mask, :register_id)");

            query.bindValue(":name", bitFieldElement.attribute("text"));
            query.bindValue(":short_name", bitFieldElement.attribute("name"));
            query.bindValue(":mask", bitFieldElement.attribute("mask"));
            query.bindValue(":register_id", registerId);

            if (!query.exec()) {
                sqlError(&query);
            } else {
                QSqlQuery lastIdQuery;
                if (lastIdQuery.exec("SELECT last_insert_rowid()")) {
                    lastIdQuery.next();
                    bitFieldId = lastIdQuery.value(0).toInt();
                    // in the case of enum bitfield add the enumerations to the bitfield_enums
                    if (bitFieldElement.attribute("enum", "") != "") {
                        QDomNodeList enumNodesList = registersNode.parentNode().childNodes(); // child of modules
                        for (int k = 0; k<enumNodesList.count(); k++) {
                            QDomElement enumElement = enumNodesList.item(k).toElement();
                            QSqlQuery enumElementAddQuery;
                            if ((enumElement.tagName() == "enumerator") &&
                                (enumElement.attribute("name") == bitFieldElement.attribute("enum", ""))) {
                                QString insertSql = "INSERT INTO bitfields_enums (name, value, text, bitfield_id) VALUES ";
                                for (int l = 0; l<enumElement.childNodes().count(); l++) { // loop through the enum's items
                                    QDomElement enumItemElement = enumElement.childNodes().item(l).toElement();
                                    bool ok = false;
                                    QString text = enumItemElement.attribute("text");
                                    int val = enumItemElement.attribute("val").remove("0x").toInt(&ok,16);
                                    if (ok) {
                                        QString valuesString = QString("(\"%1\", \"%2\", \"%3\", %4),")
                                                .arg(enumElement.attribute("name"))
                                                .arg(val)
                                                .arg(text)
                                                .arg(bitFieldId);
                                        insertSql.append(valuesString);
                                    } else {
                                        qWarning() << tr("%1 is not a hexadecimal number").arg(enumItemElement.attribute("val"));
                                    }
                                }
                                insertSql[insertSql.length()-1] = ';';
                                if (!enumElementAddQuery.exec(insertSql))
                                    sqlError(&enumElementAddQuery);
                                break;
                            }
                        }
                    }
                }
            }
        } // loop trough the current register's bitfields
    } // for loop
}

void XMLConverter::printTimeElapsed()
{
    qWarning() << startTime.msecsTo(QTime::currentTime());
}
