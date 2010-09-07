#include "fusedelegate.h"

QWidget *FuseDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & option,
    const QModelIndex & index) const
{
    FuseBitField field = index.model()->data(index, Qt::UserRole).value<FuseBitField>();
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


void FuseDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    FuseBitField field = index.model()->data(index, Qt::UserRole).value<FuseBitField>();
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


void FuseDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    int value = 0;
    FuseBitField field = index.model()->data(index, Qt::UserRole).value<FuseBitField>();
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

QWidget *FuseValueDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & /*option*/,
    const QModelIndex & index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    int value = index.model()->data(index).toInt();
    editor->setAutoFillBackground(true);
    switch (currentDisplayMode) {
    case Hexadecimal:
        editor->setInputMask("0xHH;_");
        editor->setText("0x"+QString::number(value, 16).rightJustified(2, '0'));
        break;
    case Decimal:
        editor->setInputMask("000\d;_");
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
    QLineEdit *cb = static_cast<QLineEdit*>(editor);
    switch (currentDisplayMode) {
    case Hexadecimal:
        cb->setInputMask("0xHH;_");
        cb->setText("0x"+QString::number(value, 16).rightJustified(2, '0'));
        break;
    case Decimal:
        cb->setInputMask("000\d;_");
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

    model->setData(index, value, Qt::EditRole);
}

void FuseValueDelegate::setDisplayMode(DisplayMode mode)
{
    currentDisplayMode = mode;
}

QWidget *LockDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & option,
    const QModelIndex & index) const
{
    LockBitField field = index.model()->data(index, Qt::UserRole).value<LockBitField>();
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


