#ifndef BITFIELDMODEL_H
#define BITFIELDMODEL_H

#include <QAbstractTableModel>
#include <QStyledItemDelegate>
#include <QDebug>

/*

    Class for representing 1 or more bits in a register.
    If it is a single bit it could be represented in the GUI as a single checkbox.
    For multi bit bitfields you can use an enumeration for displaying purposes.

*/

class BitField
{
public:
    BitField() :
        m_mask(0), m_value(0), m_isEnum(false) {}
    BitField(QString name_a, QString shortName_a, int mask_a, int value_a, bool isEnum_a) :
        m_text(name_a), m_shortName(shortName_a), m_mask(mask_a), m_value(value_a), m_isEnum(isEnum_a)
    {
        //qWarning() << "new bf name" << m_text << "isenum" << isEnum_a << "mask" << mask_a << "val" << value_a;
    }
    ~BitField() {}

    QString name() const {return m_text;}
    QString shortName() const {return m_shortName;}

    int mask() const {return m_mask;}
    bool isEnum() const {return m_isEnum;}

    void setValue(int value) {m_value = value;}
    int value() const {return m_value;}

    void addEnumValue(int id, QString name) {enumValues[id] = name;}

    QString text() const {return m_text;}

    // FIXME PPP
    QMap <int, QString> enumValues;// predefined group values & their name

private:
    QString m_text,  m_shortName;
    int m_mask;
    int m_value; // value is similar like in the XML file (right aligned), holds the masked value
    bool m_isEnum;

};

Q_DECLARE_METATYPE(BitField)

/*

  Class for representing a single byte configuration word
  It has name, short name, value and  it can have many bitfields
  As extra it has an offset field
  */

class Register
{
public:
    Register(QString name, int offset, int size) : m_name(name), m_offset(offset), m_size(size), m_value(0)
    {
        //qWarning() << "New register " << name << "of:"  <<  offset << "size:" << size;
    }
    ~Register() {}
    void setValue(quint8 val);
    void setMaskedValue(quint8 mask, quint8 value);
    quint8 value() const {return m_value;}
    QString name() const {return m_name;}

    // bitfield

    int bitFieldCount() const {return bitFields.count();}
    void addBitField(BitField fl) {bitFields.append(fl);}
    BitField bitFieldAt(int i) {return bitFields[i];}
    QList<BitField> bitFields; // FIXME PPP

private:
    QString m_name, m_shortName;
    int m_offset;
    int m_size;
    quint8 m_value;

};

class RegistersModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    RegistersModel(QObject *parent = 0) : QAbstractTableModel(parent), m_registers(NULL) {}

    void setRegisters(QList<Register*>* regs) {m_registers = regs;}

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QList<Register*> *m_registers;

public slots:
    void refresh();

signals:
    void changed();
};


class RegisterFieldsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    RegisterFieldsModel(QObject *parent = 0) : QAbstractTableModel(parent), m_registers(NULL) {}

    void setRegisters(QList<Register*> *regs) {m_registers = regs;}

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QList<Register*> *m_registers;

public slots:
    void refresh();

signals:
    void changed();
};

#endif // BITFIELDMODEL_H
