#ifndef FUSEMODEL_H
#define FUSEMODEL_H

#include <QAbstractTableModel>
#include <QMap>
#include <QStringList>

class FuseBitField
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
    explicit FuseModel(QObject *parent = 0) : QAbstractTableModel(parent) {}
    QList<FuseRegister> fuseRegs;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void reloadModel();
    bool setFuseValue(QString fuseName, quint8 value);
    QStringList getFuseNames();

signals:

public slots:

};

class FuseValuesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit FuseValuesModel(QObject *parent = 0) : QAbstractTableModel(parent) {}
    QList<FuseRegister> fuseRegs;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void reloadModel();
signals:

public slots:
    bool setFuseValue(QString fuseName, quint8 value);
};

#endif // FUSEMODEL_H
