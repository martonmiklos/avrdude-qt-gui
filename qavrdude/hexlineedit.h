#ifndef HEXLINEEDIT_H
#define HEXLINEEDIT_H

#include <QLineEdit>

class HexLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    HexLineEdit(QWidget * parent = 0) : QLineEdit(parent) {}
protected:
    void mousePressEvent(QMouseEvent *event) {emit clicked(); QLineEdit::mousePressEvent(event);}
signals:
    void clicked();
};

#endif // HEXLINEEDIT_H
