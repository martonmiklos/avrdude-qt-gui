#ifndef BITFIELDDELEGATE_H
#define BITFIELDDELEGATE_H

#include <QCheckBox>
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QSpinBox>
#include <QDebug>
#include <QLineEdit>
#include <QRegExpValidator>

#include "bitfieldmodel.h"

class BitFieldCheckBox: public QCheckBox
{
    Q_OBJECT
public:
    BitFieldCheckBox(QWidget *parent = 0) : QCheckBox(parent)
    {
        connect(this, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
    }
private slots:
    void toggledSlot(bool) {emit commitDataSignal(this);}
signals:
    void commitDataSignal(QWidget *widget);
};

class BitFieldComboBox : public QComboBox
{
    Q_OBJECT
public:
    BitFieldComboBox(QWidget *parent = 0) : QComboBox(parent)
    {
        connect(this, SIGNAL(activated(int)), this, SLOT(on_activated(int)));
    }
private slots:
    void on_activated(int) {emit commitDataSignal(this);}
signals:
    void commitDataSignal(QWidget *editor);
};

class BitFieldDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BitFieldDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}
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
    void commitSlot(QWidget *editor) {commitData(editor);}
signals:

};


class RegisterValueSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    enum InputType {
        Decimal = 10,
        HexaDecimal = 16,
        Binary = 2
    };

    RegisterValueSpinBox(QWidget *parent = 0);
    void setInputType(RegisterValueSpinBox::InputType type);
    InputType inputType() const {return m_inputType;}
protected:
    QValidator::State validate(QString &input, int &pos) const;
    int valueFromText(const QString &text) const;
    QString textFromValue(int val) const;

private:
    InputType m_inputType;
    QRegExpValidator *validator;

private slots:
    void valueChangedSlot(int) {emit commitDataSignal(this);}
signals:
    void commitDataSignal(QWidget*);
};


class RegisterValueDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit RegisterValueDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}
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
    void setInputType(RegisterValueSpinBox::InputType type) {m_inputType = type;}

private:
    RegisterValueSpinBox::InputType m_inputType;
private slots:
    void commitSlot(QWidget *editor) {commitData(editor);}
signals:

};

#endif // BITFIELDDELEGATE_H
