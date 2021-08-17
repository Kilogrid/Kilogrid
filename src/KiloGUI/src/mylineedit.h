#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>

class MyLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    MyLineEdit();
    void keyPressEvent(QKeyEvent * event);
    void setValues(int left, int right);
    int getLeft();
    int getRight();

signals:
    void myValueChanged();

private:
    int left;
    int right;
    bool add_to_left;
};

#endif // MYLINEEDIT_H
