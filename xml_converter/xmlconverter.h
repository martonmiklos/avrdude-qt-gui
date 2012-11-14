#ifndef XMLCONVERTER_H
#define XMLCONVERTER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtXml/QDomDocument>
#include <QtXml/QDomNode>
#include <QFile>
#include <QFileInfo>
#include <QTime>


class XMLConverter : public QObject
{
    Q_OBJECT
public:
    explicit XMLConverter(QObject *parent = 0);
    bool convert(QString sqlite, QString xmlsDir);
    QString error() const {return errorString;}
signals:
    void logMessage(QString msg);
public slots:

private:
    void sqlError(QSqlQuery *query);
    bool parseFile(QString file);

    QFile domFile;
    QDomDocument domDoc;
    QString errorString;
    QSqlDatabase database;

    QTime startTime;

    void parseRegisterDetailsFromNode(QDomNode registersNode, int deviceId, QSqlQuery &query);
    void printTimeElapsed();
};

#endif // XMLCONVERTER_H
