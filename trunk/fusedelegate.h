#ifndef FUSEDELEGATE_H
#define FUSEDELEGATE_H

#include <QCheckBox>
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QDebug>

#include "fusemodel.h"
class FuseDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit FuseDelegate(QObject *parent = 0);
    //void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const;

     void setEditorData(QWidget *editor, const QModelIndex &index) const;
     void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;

     void updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &index) const;
signals:

public slots:

};

#endif // FUSEDELEGATE_H
