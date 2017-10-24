#include "bitfieldmodel.h"
#include <QDebug>



void Register::setValue(quint8 val)
{
    for (int i = 0; i<bitFields.size(); i++) {
        if ((m_value & bitFields[i].mask()) != (val & bitFields[i].mask())) { // modify only if changed
            quint8 offset = 0;
            for (; offset < 8; offset++) {
                if (bitFields[i].mask() & (1<<offset)) {
                    break;
                }
            }
            bitFields[i].setValue((val & bitFields[i].mask()) >> offset);
        }
    }
    m_value = val;
}

void Register::setMaskedValue(quint8 mask, quint8 value)
{
    quint8 nvalue = 0;
    quint8 offset = 0;
    for (; offset<8; offset++) {
        if (mask & (1<<offset))
            break;
    }
    nvalue = (m_value & ~mask) | (mask & (value << offset));
    setValue(nvalue);
}

/* Registersmodel members: */

int RegistersModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    if (m_registers != NULL) {
        return m_registers->size();
    }

    return 0;
}
int RegistersModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant RegistersModel::data(const QModelIndex &index, int role) const
{
    if (m_registers != NULL) {
        if (role == Qt::DisplayRole) {
            if (index.row() < m_registers->size()) {
                if (index.column() == 0) {
                    return m_registers->at(index.row())->name();
                } else if (index.column() == 1) {
                    return m_registers->at(index.row())->value();
                }
            }
        }
    }
    return QVariant();
}

bool RegistersModel::setData(const QModelIndex &index, const QVariant &value, int )
{
    if (m_registers != NULL) {
        if (index.row() < m_registers->size() && index.column() == 1) {
            m_registers->at(index.row())->setValue(value.toInt());
            emit dataChanged(index, index);
            emit changed();
        }
    }
    return false;
}

QVariant RegistersModel::headerData(int section, Qt::Orientation orientation, int role) const
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
    return QVariant();
}

Qt::ItemFlags RegistersModel::flags(const QModelIndex &index) const
{
    if (index.column() == 1) {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }
    return Qt::ItemIsEnabled;
}

void RegistersModel::setRegisters(QList<Register *> *regs)
{
    m_registers = regs;
}

void RegistersModel::refresh()
{
    /*if (m_registers == NULL)
        return;

    beginRemoveRows(this->index(0,0), 0, m_registerCnt);
    endRemoveRows();

    m_registerCnt = m_registers->size();

    beginInsertRows(index(0,0), 0, m_registerCnt);
    endInsertRows();*/
    emit dataChanged(createIndex(0, 0), createIndex(rowCount()-1, columnCount()));
}

/* RegisterFieldsModel members: */

QVariant RegisterFieldsModel::headerData(int section, Qt::Orientation orientation, int role) const
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
    return QVariant();
}


Qt::ItemFlags RegisterFieldsModel::flags(const QModelIndex &index) const
{
    if (index.column() == 1) {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }
    return Qt::ItemIsEnabled;
}

QVariant RegisterFieldsModel::data(const QModelIndex &index, int role) const
{
    int rowCounter = 0;
    for (int i = 0; i<m_registers->count(); i++) {
        for (int j = 0; j<m_registers->at(i)->bitFields.count(); j++) {
            if (rowCounter == index.row()) {
                switch (index.column()) {
                case 0: { // fuse name
                        switch(role) {
                        case Qt::ToolTipRole:
                            return m_registers->at(i)->bitFields[j].name();
                            break;
                        case Qt::DisplayRole:
                            return m_registers->at(i)->bitFields[j].name() + " (" + m_registers->at(i)->bitFields[j].shortName() + ")";
                            break;
                        }; // switch role
                    } break; // column 0
                case 1: { // fuse value
                        if (role == Qt::UserRole) {
                            return QVariant::fromValue(m_registers->at(i)->bitFields[j]);
                        } else if (role ==  Qt::DisplayRole) {
                            return m_registers->at(i)->bitFields[j].value();
                        }
                    } break; // column 1
                } // switch index
                break;
            } // we have found the searched bitfield
            rowCounter++;
        } // this loops on the fuses bitfields
    } // this cycle loops on the (HIGH LOW EXTENDED FUSE) m_registers
    return QVariant();
}

bool RegisterFieldsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int rowCounter = 0;
    if ((role == Qt::DisplayRole)  || (role == Qt::EditRole)) {
        if (index.column() == 1) { //is the index points to the fuse value column?
            for (int i = 0; i<m_registers->count(); i++) {// this cycle loops on the (HIGH LOW EXTENDED FUSE) m_registers
               for (int j = 0; j<m_registers->at(i)->bitFields.count(); j++) { // this loops on the fuses bitfields
                    if (rowCounter == index.row()) {
                        m_registers->at(i)->setMaskedValue(m_registers->at(i)->bitFields.at(j).mask(), value.toInt());
                        emit changed();
                        return true;
                    } // we have found the searched bitfield
                    rowCounter++;
                } // this loops on the fuses bitfields
            } // this cycle loops on the (HIGH LOW EXTENDED FUSE) m_registers
        }
    }
    return false;
}

void RegisterFieldsModel::clear()
{
    beginRemoveRows(createIndex(0,0), 0, rowCount());
    removeRows(0, rowCount());
    endRemoveRows();
}

void RegisterFieldsModel::setRegisters(QList<Register *> *regs)
{
    m_registers = regs;
    if (m_registers == NULL)
        return;

    beginInsertRows(index(0,0), 0, m_registers->size());
    for (int i = 0; i<m_registers->size(); i++)
        insertRow(i);
    endInsertRows();
}

int RegisterFieldsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    int rowCount = 0;
    for (int i = 0; i<m_registers->count(); i++) {
        rowCount +=  m_registers->at(i)->bitFields.count();
    }
    return rowCount;
}

int RegisterFieldsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}
