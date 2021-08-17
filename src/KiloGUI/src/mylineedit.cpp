#include "mylineedit.h"

MyLineEdit::MyLineEdit()
{
    add_to_left = true;
}

void MyLineEdit::keyPressEvent(QKeyEvent * event) {
    switch(event->key()) {
        case Qt::Key_Up:
            if(add_to_left) {
                left ++;
            }
            else {
                right++;
            }

            add_to_left = !add_to_left;
            emit myValueChanged();
            break;

        case Qt::Key_Down:
            if(add_to_left) {
                right --;
            }
            else {
                left--;
            }

            add_to_left = !add_to_left;
            emit myValueChanged();
            break;

        case Qt::Key_Left:
            right++;
            left--;
            emit myValueChanged();
            break;

        case Qt::Key_Right:
            left++;
            right--;
            emit myValueChanged();
            break;
    }

    QString new_value = QString("left: %1 , right: %2").arg(left).arg(right);
    this->setText(new_value);
}

int MyLineEdit::getLeft() {
    return left;
}

int MyLineEdit::getRight() {
    return right;
}

void MyLineEdit::setValues(int left, int right) {
    this->left = left;
    this->right = right;
    //QString new_value = QString("left: %1 , right: %2").arg(left).arg(right);
    //this->setText(new_value);
    emit myValueChanged();
}
