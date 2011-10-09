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
        editor->setChecked(field.value() != 0);
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
