#ifndef FUSEDELEGATE_H
#define FUSEDELEGATE_H

#include <QCheckBox>
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QDebug>
#include <QLineEdit>

#include "fusemodel.h"
class FuseDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit FuseDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &) const
    {
        editor->setGeometry(option.rect);
    }
signals:

public slots:

};

class FuseValueDelegate : public QStyledItemDelegate
{
    typedef enum {
        Hexadecimal,
        Binary,
        Decimal
    }DisplayMode;

    Q_OBJECT
public:
    explicit FuseValueDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &) const
    {
        editor->setGeometry(option.rect);
    }

    void setDisplayMode(DisplayMode mode);
private:
    DisplayMode currentDisplayMode;
signals:

public slots:
};

class LockDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit LockDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &) const
    {
        editor->setGeometry(option.rect);
    }
signals:

public slots:

};


#endif // FUSEDELEGATE_H
