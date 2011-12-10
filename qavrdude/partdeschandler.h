#ifndef PARTDESCHANDLER_H
#define PARTDESCHANDLER_H

#include <QObject>
#include <QtXml>
class PartDescHandler : public QObject
{
Q_OBJECT

private:
    QDomDocument xmlFile;
public:
    explicit PartDescHandler(QObject *parent = 0);


signals:

public slots:

};

#endif // PARTDESCHANDLER_H
