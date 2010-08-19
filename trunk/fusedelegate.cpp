#include "fusedelegate.h"
#include <QPainter>

FuseDelegate::FuseDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

/*void FuseDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    FuseBitField field = index.model()->data(index, Qt::UserRole).value<FuseBitField>();
    if (field.isEnum) {
        QCheckBox *editor = new QCheckBox();
        editor->setGeometry(option.rect);
        editor->render(painter);
    } else {
        QComboBox *editor = new QComboBox();
        editor->setGeometry(option.rect);
        qWarning() << "paint";
        editor->render(painter);
    }
    painter->restore();
    QStyledItemDelegate::paint(painter, option, index);
}*/

QWidget *FuseDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & option,
    const QModelIndex & index) const
{
    FuseBitField field = index.model()->data(index, Qt::UserRole).value<FuseBitField>();
    if (field.isEnum) {
        QComboBox *editor = new QComboBox(parent);
        editor->setAutoFillBackground(true);
        int current = 0;
        QMapIterator<QString, int> i(field.enumValues);
        while (i.hasNext()) {
            i.next();
            editor->addItem(i.key(), i.value());
            if (field.value == i.value())
                current = i.value();
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
        int current = 0;
        QMapIterator<QString, int> i(field.enumValues);
        while (i.hasNext()) {
            i.next();
            cb->addItem(i.key(), i.value());
            if (field.value == i.value())
                current = i.value();
        }
        cb->setCurrentIndex(current);
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
            value = 255;
    }
    model->setData(index, value, Qt::EditRole);
}

void FuseDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
