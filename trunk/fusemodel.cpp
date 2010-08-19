#include "fusemodel.h"
#include <QDebug>

FuseModel::FuseModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

QVariant FuseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return tr("Name");
        else
            return tr("Value");
    } else {
        return section;
    }
}

void FuseModel::reloadModel()
{
    reset();
}

Qt::ItemFlags FuseModel::flags(const QModelIndex &index) const
{
    if (index.column() == 1) {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }
    return Qt::ItemIsEnabled;
}

QVariant FuseModel::data(const QModelIndex &index, int role) const
{
    int rowCounter = 0;
    for (int i = 0; i<fuseRegs.count(); i++) {
        for (int j = 0; j<fuseRegs[i].bitFields.count(); j++) {
            if (rowCounter == index.row()) {
                switch (index.column()) {
                case 0: { // fuse name
                        switch(role) {
                            case Qt::DisplayRole: return fuseRegs[i].bitFields[j].shortName; break;
                            case Qt::ToolTipRole: return fuseRegs[i].bitFields[j].text; break;
                        }; // switch role
                    } break; // column 0
                case 1: { // fuse value
                        if (role == Qt::UserRole) {
                            FuseBitField ret;
                            if (fuseRegs[i].bitFields[j].enumValues.isEmpty()) {
                                ret.isEnum = false;
                                return QVariant::fromValue(ret);
                            } else {
                                ret.isEnum = true;
                                ret.enumValues = fuseRegs[i].bitFields[j].enumValues;
                                return QVariant::fromValue(ret);
                            }
                        } else if (role ==  Qt::DisplayRole) {
                            return fuseRegs[i].bitFields[j].value;
                        }
                    } break; // column 1
                } // switch index
                break;
            } // we have found the searched bitfield
            rowCounter++;
        } // this loops on the fuses bitfields
    } // this cycle loops on the (HIGH LOW EXTENDED FUSE) fuseregs
    return QVariant();
}

bool FuseModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int rowCounter = 0;
    if ((role == Qt::DisplayRole)  || (role == Qt::EditRole)) {
        if (index.column() == 1) { //is the index points to the fuse value column?
            for (int i = 0; i<fuseRegs.count(); i++) {// this cycle loops on the (HIGH LOW EXTENDED FUSE) fuseregs
                for (int j = 0; j<fuseRegs[i].bitFields.count(); j++) { // this loops on the fuses bitfields
                    if (rowCounter == index.row()) {
                        fuseRegs[i].bitFields[j].value = value.toInt();
                        return true;
                    } // we have found the searched bitfield
                    rowCounter++;
                } // this loops on the fuses bitfields
            } // this cycle loops on the (HIGH LOW EXTENDED FUSE) fuseregs
        }
    }
    return false;
}

int FuseModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    int rowCount = 0;
    for (int i = 0; i<fuseRegs.count(); i++) {
        rowCount += fuseRegs[i].bitFields.count();
    }
    return rowCount;
}

int FuseModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}

