#include "fusemodel.h"
#include <QDebug>

QVariant FuseModelCute::headerData(int section, Qt::Orientation orientation, int role) const
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

void FuseModelCute::reloadModel()
{
    this->dataChanged(this->index(1,0), this->index(1, this->rowCount(this->index(-1, -1))));
    this->reset();

}

Qt::ItemFlags FuseModelCute::flags(const QModelIndex &index) const
{
    if (index.column() == 1) {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }
    return Qt::ItemIsEnabled;
}

QVariant FuseModelCute::data(const QModelIndex &index, int role) const
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

bool FuseModelCute::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int rowCounter = 0;
    if ((role == Qt::DisplayRole)  || (role == Qt::EditRole)) {
        if (index.column() == 1) { //is the index points to the fuse value column?
            for (int i = 0; i<part->fuseRegs.count(); i++) {// this cycle loops on the (HIGH LOW EXTENDED FUSE) part->fuseRegs
                for (int j = 0; j<part->fuseRegs[i].bitFields.count(); j++) { // this loops on the fuses bitfields
                    if (rowCounter == index.row()) {
                        qWarning() << "mask" << QString::number(part->fuseRegs[i].bitFields[j].mask, 2) << "val" << value;
                        qWarning() << part->fuseRegs[i].value << QString::number(part->fuseRegs[i].value, 2);
                        part->fuseRegs[i].bitFields[j].value = value.toInt();
                        part->fuseRegs[i].value &= ~part->fuseRegs[i].bitFields[j].mask;
                        int l;
                        for (l = 0; l<8; l++) {
                            if (part->fuseRegs[i].bitFields[j].mask & (1<<l)) {
                                break;
                            }
                        }
                        part->fuseRegs[i].value |= part->fuseRegs[i].bitFields[j].value * (1<<l);
                        qWarning() << part->fuseRegs[i].value << QString::number(part->fuseRegs[i].value, 2);
                        return true;
                    } // we have found the searched bitfield
                    rowCounter++;
                } // this loops on the fuses bitfields
            } // this cycle loops on the (HIGH LOW EXTENDED FUSE) part->fuseRegs
        }
    }
    return false;
}

int FuseModelCute::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    int rowCount = 0;
    for (int i = 0; i<part->fuseRegs.count(); i++) {
        rowCount += part->fuseRegs[i].bitFields.count();
    }
    return rowCount;
}

int FuseModelCute::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}

int FuseValuesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    if (currentMode == FuseBitDisplayMode_BinaryDetailed)
        return part->fuseRegs.size()*2;
    return part->fuseRegs.size();
}

int FuseValuesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    if (currentMode == FuseBitDisplayMode_BinaryDetailed)
        return 9;
    return 2;
}

QVariant FuseValuesModel::data(const QModelIndex &index, int role) const
{
    if (currentMode == FuseBitDisplayMode_BinaryDetailed) {
        if (index.row()%2 ==  1) { // value row
            if ((index.column() == 0) && (role == Qt::DisplayRole)) {
                return part->fuseRegs[(index.row() - 1)/2].name;
            } else {
                return (bool)(part->fuseRegs[(index.row() - 1)/2].value & (1<<(index.column()-1)));
            }
        } else { // bits name row
            if (((index.column() > 0) && (index.column() < 9)) && (role == Qt::DisplayRole)) {
                return part->getFuseRegisterBitName((index.row())/2, index.column()-1);
            }
        }
    } else {
        if (index.row() < part->fuseRegs.size()) {
            if ((index.column() == 0) && (role == Qt::DisplayRole)) {
                return part->fuseRegs[index.row()].name;
            }

            if ((index.column() == 1) && (role == Qt::DisplayRole)) {
                return part->fuseRegs[index.row()].value;
            }
        }
    }
    return QVariant();
}

bool FuseValuesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < part->fuseRegs.size()) {
        if ((index.column() == 1) && (role == Qt::DisplayRole)){
            part->fuseRegs[index.row()].value = value.toInt();
            return true;
        }
    }
    return false;
}

QVariant FuseValuesModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags FuseValuesModel::flags(const QModelIndex &index) const
{
    if (currentMode == FuseBitDisplayMode_BinaryDetailed) {
        if (index.row() %2 == 0) {
            return Qt::ItemIsEnabled;
        } else {
            if (index.column() > 0) {
                return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
            } else {
                qWarning() << "1st" << index;
                return Qt::ItemIsEnabled;
            }
        }
    } else {
        if (index.column() == 1) {
            return Qt::ItemIsEditable | Qt::ItemIsEnabled;
        }
    }
    return Qt::ItemIsEnabled;
}

void FuseValuesModel::reloadModel()
{
    reset();
}

void FuseValuesModel::setDisplayMode(FuseBitDisplayMode mode)
{
    currentMode = mode;
    if (mode == FuseBitDisplayMode_BinaryDetailed) {
        emit dataChanged(this->index(0,0), this->index(part->fuseRegs.size()*2, 9));
    } else {

    }
    this->reset();
}

