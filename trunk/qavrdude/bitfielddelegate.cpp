#include "bitfielddelegate.h"



QWidget *BitFieldDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & option,
    const QModelIndex & index) const
{
    BitField field = index.model()->data(index, Qt::UserRole).value<BitField>();
    if (field.isEnum()) {
        BitFieldComboBox *editor = new BitFieldComboBox(parent);
        connect(editor, SIGNAL(commitDataSignal(QWidget*)), this, SLOT(commitSlot(QWidget*)));
        editor->setAutoFillBackground(true);
        int current = 0;
        QMapIterator<int, QString> i(field.enumValues);
        while (i.hasNext()) {
            i.next();
            editor->addItem(i.value(), i.key());
            if (field.value() == i.key())
                current = i.key();
        }
        editor->setCurrentIndex(current);
        return editor;
    } else {
        BitFieldCheckBox *editor = new BitFieldCheckBox(parent);
        connect(editor, SIGNAL(commitDataSignal(QWidget*)), this, SIGNAL(commitData(QWidget*)));
        editor->setAutoFillBackground(true);
        editor->setChecked(field.value() == 0);
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}


void BitFieldDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    BitField field = index.model()->data(index, Qt::UserRole).value<BitField>();
    if (field.isEnum()) {
        BitFieldComboBox *cb = static_cast<BitFieldComboBox*>(editor);
        cb->setAutoFillBackground(true);
        cb->setCurrentIndex(cb->findData(field.value()));
    } else {
        BitFieldCheckBox *cb = static_cast<BitFieldCheckBox*>(editor);
        cb->setAutoFillBackground(true);
        cb->setChecked(field.value() != 0);
    }
}


void BitFieldDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    int value = 0;
    BitField field = index.model()->data(index, Qt::UserRole).value<BitField>();
    if (field.isEnum()) {
        BitFieldComboBox *cb = static_cast<BitFieldComboBox*>(editor);
        value = cb->itemData(cb->currentIndex()).toInt();
    } else {
        BitFieldCheckBox *cb = static_cast<BitFieldCheckBox*>(editor);
        if (cb->isChecked())
            value = 1;
    }
    model->setData(index, value, Qt::EditRole);
}

RegisterValueSpinBox::RegisterValueSpinBox(QWidget *parent)
    : QSpinBox(parent)
{
    setRange(0, 255);
    validator = new QRegExpValidator(this);
    setInputType(HexaDecimal);
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
}

void RegisterValueSpinBox::setInputType(RegisterValueSpinBox::InputType type)
{
    switch (type) {
    case Decimal:
        validator->setRegExp(QRegExp("[0-9]{1,3}"));
        setPrefix("");
        break;
    case HexaDecimal:
        setPrefix("0x");
        validator->setRegExp(QRegExp("[0-9A-Fa-f]{1,2}"));
        break;
    case Binary:
        setPrefix("0b");
        validator->setRegExp(QRegExp("[0-1]{1,8}"));
        break;
    }
    m_inputType = type;
}

QValidator::State RegisterValueSpinBox::validate(QString &input, int &pos) const
{
    return validator->validate(input, pos);
}

QString RegisterValueSpinBox::textFromValue(int val) const
{
    return QString::number(val, (int)m_inputType);
}

int RegisterValueSpinBox::valueFromText(const QString &text) const
{
    bool ok;
    return text.toInt(&ok, (int)m_inputType);
}


QWidget * RegisterValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    RegisterValueSpinBox *editor = new RegisterValueSpinBox(parent);
    qWarning() << m_inputType;
    editor->setInputType(m_inputType);
    connect(editor, SIGNAL(commitDataSignal(QWidget*)), this, SIGNAL(commitData(QWidget*)));
    editor->setAutoFillBackground(true);
    return editor;
}

void RegisterValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    RegisterValueSpinBox *cb = static_cast<RegisterValueSpinBox*>(editor);
    cb->setInputType(m_inputType);
    cb->setValue(index.model()->data(index).toInt());
}

void RegisterValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    RegisterValueSpinBox *cb = static_cast<RegisterValueSpinBox*>(editor);
    model->setData(index, cb->value(), Qt::EditRole);
}
