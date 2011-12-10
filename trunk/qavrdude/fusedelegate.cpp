#include "fusedelegate.h"


QWidget *FuseDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & option,
    const QModelIndex & index) const
{
    BitField field = index.model()->data(index, Qt::UserRole).value<BitField>();
    if (field.isEnum()) {
        FuseComboBox *editor = new FuseComboBox(parent);
        connect(editor, SIGNAL(activated(int)), this, SLOT(commitSlot()));
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
        FuseCheckBox *editor = new FuseCheckBox(parent);
        connect(editor, SIGNAL(commitDataSignal(QWidget*)), this, SIGNAL(commitData(QWidget*)));
        editor->setAutoFillBackground(true);
        editor->setChecked(field.value() == 0);
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}


void FuseDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    BitField field = index.model()->data(index, Qt::UserRole).value<BitField>();
    if (field.isEnum()) {
        FuseComboBox *cb = static_cast<FuseComboBox*>(editor);
        cb->setAutoFillBackground(true);
        cb->setCurrentIndex(cb->findData(field.value()));
    } else {
        FuseCheckBox *cb = static_cast<FuseCheckBox*>(editor);
        cb->setAutoFillBackground(true);
        cb->setChecked(field.value() == 0);
    }
}


void FuseDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    int value = 0;
    BitField field = index.model()->data(index, Qt::UserRole).value<BitField>();
    if (field.isEnum()) {
        FuseComboBox *cb = static_cast<FuseComboBox*>(editor);
        value = cb->itemData(cb->currentIndex()).toInt();
    } else {
        FuseCheckBox *cb = static_cast<FuseCheckBox*>(editor);
        if (cb->isChecked())
            value = 1;
    }
    model->setData(index, value, Qt::EditRole);
}

QWidget *FuseValueDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & /*option*/,
    const QModelIndex & index) const
{
    FuseValueLineEdit *editor = new FuseValueLineEdit(parent);
    int value = index.model()->data(index).toInt();
    editor->setAutoFillBackground(true);
    switch (currentDisplayMode) {
    case Hexadecimal:
        editor->setInputMask("0x>HH;_");
        editor->setText("0x"+QString::number(value, 16).rightJustified(2, '0').toUpper());
        break;
    case Decimal:
        editor->setInputMask("000;_");
        editor->setText(QString::number(value, 10)+"d");
        break;
    case Binary:
        editor->setInputMask("\0\bBBBBBBBB;_");
        editor->setText("0b"+QString::number(value, 2).rightJustified(8, '0'));
        break;
    }
    return editor;
}


void FuseValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int value = index.model()->data(index).toInt();
    FuseValueLineEdit *cb = static_cast<FuseValueLineEdit*>(editor);
    switch (currentDisplayMode) {
    case Hexadecimal:
        cb->setInputMask("0xHH;_");
        cb->setText("0x"+QString::number(value, 16).rightJustified(2, '0').toUpper());
        break;
    case Decimal:
        cb->setInputMask("000;_");
        cb->setText(QString::number(value, 10)+"d");
        break;
    case Binary:
        cb->setInputMask("\0\bBBBBBBBB;_");
        cb->setText("0b"+QString::number(value, 2).rightJustified(8, '0'));
        break;
    }
}


void FuseValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    int value = 0;
    bool ok = false;
    FuseValueLineEdit *le = static_cast<FuseValueLineEdit*>(editor);
    QString valueText = le->text();
    switch (currentDisplayMode) {
    case Hexadecimal:
        valueText = valueText.mid(2);
        value = valueText.toInt(&ok, 16);
        break;
    case Decimal:
        valueText = valueText.left(valueText.size()-1);
        value = valueText.toInt(&ok, 10);
        break;
    case Binary:
        valueText = valueText.mid(2);
        value = valueText.toInt(&ok, 2);
        break;
    }
    if ((ok) && (value < 256)) {
        model->setData(index, value, Qt::EditRole);
    }
}

void FuseValueDelegate::setDisplayMode(FuseValueDisplayMode mode)
{
    currentDisplayMode = mode;
}

QWidget *LockDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & option,
    const QModelIndex & index) const
{
    BitField field = index.model()->data(index, Qt::UserRole).value<LockBitField>();
    if (field.isEnum) {
        QComboBox *editor = new QComboBox(parent);
        editor->setAutoFillBackground(true);
        int current = 0;
        QMapIterator<int, QString> i(field.enumValues);
        while (i.hasNext()) {
            i.next();
            editor->addItem(i.value(), i.key());
            if (field.value == i.key())
                current = i.key();
        }
        editor->setCurrentIndex(current);
        return editor;
    } else {
        QCheckBox *editor = new QCheckBox(parent);
        editor->setAutoFillBackground(true);
        editor->setChecked(field.value != 0);
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}


void LockDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    LockBitField field = index.model()->data(index, Qt::UserRole).value<LockBitField>();
    if (field.isEnum) {
        QComboBox *cb = static_cast<QComboBox*>(editor);
        cb->setAutoFillBackground(true);
        cb->setCurrentIndex(cb->findData(field.value));
    } else {
        QCheckBox *cb = static_cast<QCheckBox*>(editor);
        cb->setAutoFillBackground(true);
        cb->setChecked(field.value != 0);
    }
}


void LockDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    int value = 0;
    LockBitField field = index.model()->data(index, Qt::UserRole).value<LockBitField>();
    if (field.isEnum) {
        QComboBox *cb = static_cast<QComboBox*>(editor);
        value = cb->itemData(cb->currentIndex()).toInt();
    } else {
        QCheckBox *cb = static_cast<QCheckBox*>(editor);
        if (cb->isChecked())
            value = 1;
    }
    model->setData(index, value, Qt::EditRole);
}


