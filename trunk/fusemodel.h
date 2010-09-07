#ifndef FUSEMODEL_H
#define FUSEMODEL_H

#include <QAbstractTableModel>
#include <QMap>
#include <QStringList>

#include "avrpart.h"

class AvrPart;

class FuseBitField // represent a fusebitgroup (like CLKSEL)
{
public:
    FuseBitField(){}
    QString shortName, text;
    int mask;
    int value; // value is similar like in the XML file (right aligned)
    bool isEnum;
    QMap <int, QString> enumValues;// predefined group values & their name
};

Q_DECLARE_METATYPE(FuseBitField);

class FuseRegister // represents a fuse byte (high,low, extended)
{
public:
    FuseRegister(QString name, int offset, int size) :name(name), offset(offset), size(size){}
    QList<FuseBitField> bitFields;
    quint8 value;
    QString name;
    int offset;
    int size;
};

class FuseModelCute : public QAbstractTableModel
{
    Q_OBJECT
public:
    FuseModelCute(AvrPart *pa, QObject *parent = 0) : QAbstractTableModel(parent), part(pa) {}
    AvrPart *part;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void reloadModel();
signals:
    void changed();
};

class FuseValuesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    FuseValuesModel(AvrPart *pa, QObject *parent = 0) : QAbstractTableModel(parent), part(pa) {}
    AvrPart *part;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void reloadModel();
signals:
    void changed();
};

class LockBitsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    LockBitsModel(AvrPart *pa, QObject *parent = 0) : QAbstractTableModel(parent), part(pa) {}
    AvrPart *part;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void reloadModel();
};

#endif // FUSEMODEL_H
