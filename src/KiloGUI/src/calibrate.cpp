#include <QtWidgets>
#include <QSpinBox>
#include <QPushButton>
#include "mylineedit.h"
#include "calibrate.h"


namespace {
    constexpr bool   DEFAULT_UID_CALIBRATION   = false;
    constexpr double STRAIGHT_INITIALIZE_RATIO = 0.90;

    constexpr unsigned int UID_MIN = 1;
    constexpr unsigned int UID_MAX = 127;
    constexpr unsigned int TURN_LEFT_MIN = 1;
    constexpr unsigned int TURN_LEFT_MAX = 110;
    constexpr unsigned int TURN_RIGHT_MIN = 1;
    constexpr unsigned int TURN_RIGHT_MAX = 110;

    enum {
        INDEX_UID = 0,
        INDEX_LEFT,
        INDEX_RIGHT
    };
}


CalibWindow::CalibWindow(QString title, QWidget *parent)
    : QWidget(parent)
{
    QPushButton *save_button = new QPushButton("&Save");
    QPushButton *close_button = new QPushButton("&Close");
    QObject::connect(save_button, SIGNAL(clicked()), this, SLOT(save()));
    QObject::connect(close_button, SIGNAL(clicked()), this, SLOT(close()));

    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *hbox = new QHBoxLayout();
    QGridLayout *grid = new QGridLayout();
    hbox->addWidget(save_button);
    hbox->addWidget(close_button);
    layout->addLayout(grid);
    layout->addLayout(hbox);
    setLayout(layout);
    setWindowTitle(title);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::WindowStaysOnTopHint|Qt::Dialog);

    QPushButton *uid_label = new QPushButton("Unique ID");
    uid_label->setCheckable(true);
    uid_label->setChecked(DEFAULT_UID_CALIBRATION);
    uid_input = new QSpinBox();
    uid_input->setRange(UID_MIN, UID_MAX);
    uid_input->setSingleStep(1);
    uid_button = new QPushButton("Test");
    // [DEFAULT] ID calibration is disabled for safety.
    uid_input->setEnabled(DEFAULT_UID_CALIBRATION);
    uid_button->setEnabled(DEFAULT_UID_CALIBRATION);
    connect(uid_label, SIGNAL(toggled(bool)), this, SLOT(toggleCalibID(bool)));

    QLabel *turnleft_label = new QLabel("Turn left");
    QSpinBox *turnleft_input = new QSpinBox();
    turnleft_input->setRange(TURN_LEFT_MIN, TURN_LEFT_MAX);
    turnleft_input->setSingleStep(1);
    QPushButton *turnleft_button = new QPushButton("Test");

    QLabel *turnright_label = new QLabel("Turn right");
    QSpinBox *turnright_input = new QSpinBox();
    turnright_input->setRange(TURN_RIGHT_MIN, TURN_RIGHT_MAX);
    turnright_input->setSingleStep(1);
    QPushButton *turnright_button = new QPushButton("Test");

    QLabel *straight_label = new QLabel("Go Straight");
    straightCalib = new MyLineEdit();
    QPushButton *straight_button = new QPushButton("Init straight");

    grid->addWidget(uid_label, 0, 0);
    grid->addWidget(uid_input, 0, 1);
    grid->addWidget(uid_button, 0, 2);

    grid->addWidget(turnleft_label, 1, 0);
    grid->addWidget(turnleft_input, 1, 1);
    grid->addWidget(turnleft_button, 1, 2);

    grid->addWidget(turnright_label, 2, 0);
    grid->addWidget(turnright_input, 2, 1);
    grid->addWidget(turnright_button, 2, 2);

    grid->addWidget(straight_label, 3, 0);
    grid->addWidget(straightCalib, 3, 1);
    grid->addWidget(straight_button, 3, 2);

    QSignalMapper *signalMapper = new QSignalMapper();
    QObject::connect(uid_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
    QObject::connect(turnleft_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
    QObject::connect(turnright_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
    QObject::connect(turnleft_input, SIGNAL(valueChanged(int)), signalMapper, SLOT(map()));
    QObject::connect(turnright_input, SIGNAL(valueChanged(int)), signalMapper, SLOT(map()));

    QObject::connect(straight_button, SIGNAL(clicked()), this, SLOT(initStraightCalib()));
    QObject::connect(straightCalib, SIGNAL(myValueChanged()), this, SLOT(calibStraightValueChange()));

    signalMapper->setMapping(uid_button, 0);
    signalMapper->setMapping(turnleft_button, 1);
    signalMapper->setMapping(turnright_button, 2);
    signalMapper->setMapping(turnleft_input, 1);
    signalMapper->setMapping(turnright_input, 2);

    values[INDEX_UID]   = uid_input;
    values[INDEX_LEFT]  = turnleft_input;
    values[INDEX_RIGHT] = turnright_input;

    QObject::connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(updateCalib(int)));
}

void CalibWindow::updateCalib(int v) {
    switch(v) {
        case INDEX_UID:
            emit calibUID(values[INDEX_UID]->value());
            break;
        case INDEX_LEFT:
            emit calibLeft(values[INDEX_LEFT]->value());
            break;
        case INDEX_RIGHT:
            emit calibRight(values[INDEX_RIGHT]->value());
            break;
        default:
            break;
    }
}

void CalibWindow::closeEvent(QCloseEvent *event) {
    emit calibStop();
    event->accept();
}

void CalibWindow::save() {
    emit calibSave();
}

void CalibWindow::initStraightCalib() {
    straightCalib->setValues(values[1]->value()*STRAIGHT_INITIALIZE_RATIO, values[2]->value()*STRAIGHT_INITIALIZE_RATIO);
    straightCalib->setFocus();
    calibStraightValueChange();
}

void CalibWindow::calibStraightValueChange() {
    emit calibStraight(straightCalib->getLeft() | ((straightCalib->getRight())&0xFF)<<8);
}

void CalibWindow::toggleCalibID(bool flag)
{
    uid_input->setEnabled(flag);
    uid_button->setEnabled(flag);
}
