#ifndef FUSEDELEGATE_H
#define FUSEDELEGATE_H

#include <QCheckBox>
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QDebug>
#include <QLineEdit>
#include "bitfieldmodel.h"

class FuseCheckBox: public QCheckBox
{
    Q_OBJECT
public:
    FuseCheckBox(QWidget *parent = 0) : QCheckBox(parent)
    {
        connect(this, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
    }
private slots:
    void toggledSlot(bool) {emit commitDataSignal(this);}
signals:
    void commitDataSignal(QWidget *widget);
};

class FuseComboBox : public QComboBox
{
    Q_OBJECT
public:
    FuseComboBox(QWidget *parent = 0) : QComboBox(parent)
    {
        connect(this, SIGNAL(activated(int)), this, SLOT(on_activated(int)));
    }
private slots:
    void on_activated(int) {emit commitDataSignal(this);}
signals:
    void commitDataSignal(QWidget *editor);
};

class FuseValueLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    FuseValueLineEdit(QWidget *parent = 0) : QLineEdit(parent)
    {
        connect(this, SIGNAL(textEdited(QString)), this, SLOT(on_textEdited(QString)));
    }
private slots:
    void on_textEdited(QString) {emit commitDataSignal(this);}
signals:
    void commitDataSignal(QWidget *editor);
};

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

private slots:
    void commitSlot() {}
signals:

};

typedef enum {
    Hexadecimal = 0,
    Binary = 1,
    Decimal = 2
          } FuseValueDisplayMode;

class FuseValueDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit FuseValueDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {currentDisplayMode = Hexadecimal;}
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

    void setDisplayMode(FuseValueDisplayMode mode);
private:
    FuseValueDisplayMode currentDisplayMode;
signals:
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
