#include "fusemodel.h"
#include <QDebug>

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

bool FuseModel::setFuseValue(QString fuseName, quint8 value)
{
    foreach (FuseRegister fr, fuseRegs) {
        if (fr.name == fuseName) {
            fr.value = value;
            return true;
        }
    }
    return false;
}

QStringList FuseModel::getFuseNames()
{
    QStringList ret;
    foreach (FuseRegister fr, fuseRegs) {
        ret.append(fr.name);
    }
    return ret;
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

int FuseValuesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return fuseRegs.size();
}

int FuseValuesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}

QVariant FuseValuesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < fuseRegs.size()) {
        if ((index.column() == 0) && (role == Qt::DisplayRole)) {
            return fuseRegs[index.row()].name;
        }

        if ((index.column() == 1) && (role == Qt::DisplayRole)) {
            return fuseRegs[index.row()].value;
        }
    }
    return QVariant();
}

bool FuseValuesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < fuseRegs.size()) {
        if (index.column() == 1) {
            fuseRegs[index.row()].value = value.toInt();
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
    if (index.column() == 1) {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }
    return Qt::ItemIsEnabled;
}
void FuseValuesModel::reloadModel()
{
    reset();
}

bool FuseValuesModel::setFuseValue(QString fuseName, quint8 value)
{
    for(int i = 0; i< fuseRegs.size(); i++) {
        if (fuseRegs[i].name == fuseName) {
            fuseRegs[i].value = value;
            qWarning() << fuseName << value;
            emit dataChanged(this->index(i, 1), this->index(i,1));
            return true;
        }
    }
    return false;
}
