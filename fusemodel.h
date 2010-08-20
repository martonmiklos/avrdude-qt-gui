#ifndef FUSEMODEL_H
#define FUSEMODEL_H

#include <QAbstractTableModel>
#include <QMap>
#include <QStringList>

#include "avrpart.h"

class AvrPart;

typedef enum {
    FuseBitDisplayMode_Decimal,
    FuseBitDisplayMode_Hexadecimal,
    FuseBitDisplayMode_Binary,
    FuseBitDisplayMode_BinaryDetailed
} FuseBitDisplayMode;

class FuseBitField // represent a
{
public:
    FuseBitField(){}
    QString shortName, text;
    int mask;
    int value;
    bool isEnum;
    QMap <QString, int> enumValues;
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

class FuseModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    FuseModel(AvrPart *pa, QObject *parent = 0) : QAbstractTableModel(parent), part(pa) {}
    AvrPart *part;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void reloadModel();
};

class FuseValuesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    FuseValuesModel(AvrPart *pa, QObject *parent = 0) : QAbstractTableModel(parent), part(pa), currentMode(FuseBitDisplayMode_Decimal) {}
    AvrPart *part;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void reloadModel();
    void setDisplayMode(FuseBitDisplayMode mode);

private:
    FuseBitDisplayMode currentMode;

};

#endif // FUSEMODEL_H
