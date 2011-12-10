#include "fusemodel.h"
#include <QDebug>

QVariant BitFieldModel::headerData(int section, Qt::Orientation orientation, int role) const
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

void BitFieldModel::reloadModel()
{
    this->dataChanged(this->index(1,0), this->index(1, this->rowCount(this->index(-1, -1))-1));
}

Qt::ItemFlags BitFieldModel::flags(const QModelIndex &index) const
{
    if (index.column() == 1) {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }
    return Qt::ItemIsEnabled;
}

QVariant BitFieldModel::data(const QModelIndex &index, int role) const
{
    int rowCounter = 0;
    for (int i = 0; i<part->fuseRegs.count(); i++) {
        for (int j = 0; j<part->fuseRegs[i].bitFields.count(); j++) {
            if (rowCounter == index.row()) {
                switch (index.column()) {
                case 0: { // fuse name
                        switch(role) {
                        case Qt::DisplayRole: return part->fuseRegs[i].bitFields[j].shortName; break;
                        case Qt::ToolTipRole: return part->fuseRegs[i].bitFields[j].text; break;
                        }; // switch role
                    } break; // column 0
                case 1: { // fuse value
                        if (role == Qt::UserRole) {
                            return QVariant::fromValue(part->fuseRegs[i].bitFields[j]);
                        } else if (role ==  Qt::DisplayRole) {
                            return part->fuseRegs[i].bitFields[j].value;
                        }
                    } break; // column 1
                } // switch index
                break;
            } // we have found the searched bitfield
            rowCounter++;
        } // this loops on the fuses bitfields
    } // this cycle loops on the (HIGH LOW EXTENDED FUSE) part->fuseRegs
    return QVariant();
}

bool BitFieldModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int rowCounter = 0;
    if ((role == Qt::DisplayRole)  || (role == Qt::EditRole)) {
        if (index.column() == 1) { //is the index points to the fuse value column?
            for (int i = 0; i<part->fuseRegs.count(); i++) {// this cycle loops on the (HIGH LOW EXTENDED FUSE) part->fuseRegs
                for (int j = 0; j<part->fuseRegs[i].bitFields.count(); j++) { // this loops on the fuses bitfields
                    if (rowCounter == index.row()) {
                        part->fuseRegs[i].bitFields[j].value = value.toInt();
                        part->fuseRegs[i].value &= ~part->fuseRegs[i].bitFields[j].mask;
                        int l;
                        for (l = 0; l<8; l++) {
                            if (part->fuseRegs[i].bitFields[j].mask & (1<<l)) {
                                break;
                            }
                        }
                        part->fuseRegs[i].value |= part->fuseRegs[i].bitFields[j].value * (1<<l);
                        emit changed();
                        return true;
                    } // we have found the searched bitfield
                    rowCounter++;
                } // this loops on the fuses bitfields
            } // this cycle loops on the (HIGH LOW EXTENDED FUSE) part->fuseRegs
        }
    }
    return false;
}

int BitFieldModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    int rowCount = 0;
    for (int i = 0; i<part->fuseRegs.count(); i++) {
        rowCount += part->fuseRegs[i].bitFields.count();
    }
    return rowCount;
}

int BitFieldModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}

int BitFieldValueModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return part->fuseRegs.size();
}

int BitFieldValueModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}

QVariant BitFieldValueModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < part->fuseRegs.size()) {
        if ((index.column() == 0) && (role == Qt::DisplayRole)) {
            return part->fuseRegs[index.row()].name;
        }

        if ((index.column() == 1) && (role == Qt::DisplayRole)) {
            return (int)part->fuseRegs[index.row()].value;
        }
    }
    return QVariant();
}

bool BitFieldValueModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < part->fuseRegs.size()) {
        if ((index.column() == 1) && (role == Qt::DisplayRole)){
            part->fuseRegs[index.row()].value = value.toInt();
            emit changed();
            return true;
        }
    }
    return false;
}

QVariant BitFieldValueModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags BitFieldValueModel::flags(const QModelIndex &index) const
{
    if (index.column() == 1) {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }
    return Qt::ItemIsEnabled;
}

void BitFieldValueModel::reloadModel()
{
    this->dataChanged(this->index(1,0), this->index(1, this->rowCount(this->index(-1, -1))));
}

QVariant LockBitsModel::headerData(int section, Qt::Orientation orientation, int role) const
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

void LockBitsModel::reloadModel()
{
    this->dataChanged(this->index(1,0), this->index(1, this->rowCount(this->index(-1, -1))));
}

Qt::ItemFlags LockBitsModel::flags(const QModelIndex &index) const
{
    if (index.column() == 1) {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }
    return Qt::ItemIsEnabled;
}

QVariant LockBitsModel::data(const QModelIndex &index, int role) const
{
    int rowCounter = 0;
    for (int j = 0; j<part->lockbyte.bitFields.count(); j++) {
        if (rowCounter == index.row()) {
            switch (index.column()) {
            case 0: { // fuse name
                    switch(role) {
                    case Qt::DisplayRole: return part->lockbyte.bitFields[j].shortName; break;
                    case Qt::ToolTipRole: return part->lockbyte.bitFields[j].text; break;
                    }; // switch role
                } break; // column 0
            case 1: { // fuse value
                    if (role == Qt::UserRole) {
                        return QVariant::fromValue(part->lockbyte.bitFields[j]);
                    } else if (role ==  Qt::DisplayRole) {
                        return part->lockbyte.bitFields[j].value;
                    }
                } break; // column 1
            } // switch index
            break;
        } // we have found the searched bitfield
        rowCounter++;
    } // this loops on the fuses bitfields
    return QVariant();
}

bool LockBitsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int rowCounter = 0;
    if ((role == Qt::DisplayRole)  || (role == Qt::EditRole)) {
        if (index.column() == 1) { //is the index points to the lockbits value column?
            for (int j = 0; j<part->lockbyte.bitFields.count(); j++) { // this loops on the lockbits bitfields
                if (rowCounter == index.row()) {
                    part->lockbyte.bitFields[j].value = value.toInt();
                    part->lockbyte.value &= ~part->lockbyte.bitFields[j].mask;
                    int l;
                    for (l = 0; l<8; l++) {
                        if (part->lockbyte.bitFields[j].mask & (1<<l)) {
                            break;
                        }
                    }
                    part->lockbyte.value |= part->lockbyte.bitFields[j].value * (1<<l);
                    return true;
                } // we have found the searched bitfield
                rowCounter++;
            } // this loops on the lockregisters bitfields
        }
    }
    return false;
}

int LockBitsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return part->lockbyte.bitFields.count();
}

int LockBitsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}
