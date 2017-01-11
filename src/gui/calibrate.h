#ifndef CALIBWINDOW_H
#define CALIBWINDOW_H

#include <QWidget>

class QSpinBox;
class QPushButton;
class MyLineEdit;


class CalibWindow: public QWidget {
    Q_OBJECT

public:
    CalibWindow(QString title, QWidget *parent = 0);
    void closeEvent(QCloseEvent *);

signals:
    void calibUID(int);
    void calibLeft(int);
    void calibRight(int);
    void calibStraight(int);
    void calibSave();
    void calibStop();

public slots:
    void updateCalib(int);
    void save();
    void calibStraightValueChange();
    void initStraightCalib();
    void toggleCalibID(bool);

private:
    const QSpinBox *values[5];
    MyLineEdit     *straightCalib;
    // calib IDs
    QSpinBox       *uid_input;
    QPushButton    *uid_button;
};

#endif//CALIBWINDOW_H
