/*
 * TODO: detailed description of the class.
 * Kilobot command buttons are linked to their corresponding number in the KILOBOT_COMMANDS
 * table through a "setMapper(QObject, int)" call. Then, the "mapped(int)" signal is emitted
 * every time a Kilobot command button is clicked.
 */

// TODO bug with the Upload button - does not stay checked when uploading

#include <QtWidgets>
#include <QFileDialog>
#include <QStatusBar>
#include <QToolButton>
#include <QComboBox>
#include <QList>
#include <QTime> // for pause
#include <QDateTime>
#include "kilowindow.h"
#include <ftdi.h>
#include "serial_packet.h"
#include "message.h"
#include "kilogrid.h"
#include "configurationparser.h"
#include "trackingparser.h"

typedef struct {
    const char *name;
    const IR_message_type_t type;
} kilo_cmd_t;

static const kilo_cmd_t KILO_COMMANDS[] = {
    {"Reset", RESET},
    {"Run", RUN},
    {"Sleep", SLEEP},
    {"Voltage", VOLTAGE},
    {"Charge", CHARGE},
    {"Turn left", TURN_LEFT},
    {"Turn right", TURN_RIGHT},
    {"Go straight", GO_STRAIGHT},
};

static const int NUM_KILO_COMMANDS = sizeof(KILO_COMMANDS)/sizeof(kilo_cmd_t);

namespace {
    constexpr int SMALL_ARENA_BAUDRATE = 38400;   // used to make 'radio1' checked
    constexpr int LARGE_ARENA_BAUDRATE = 256000;  // used to make 'radio2' checked
    constexpr int DISPATCHER_BAUDRATE = BAUD;     // defined in Common/kilogui-dispatcher/serial_speed.h
}

void delay_no_freeze( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

KiloWindow::KiloWindow(QWidget *parent)
    : QWidget(parent),
      is_initialized(false),
      device(0),
      app_info(new KiloGUI_Info(QCoreApplication::applicationDirPath())), // create application information object
      kilogrid_IR_enabled(false),
      kilogrid_command_active(false),
      connected(false)                                                    // make KiloGUI disconnected to Kilogrid
{
    // Create status bar
    status = new QStatusBar();
    status->showMessage("Disconnected.");
    status->setSizeGripEnabled(false);

    // Create status bar button
    connect_button = new QToolButton(status);
    status->addPermanentWidget(connect_button);
    connect_button->setText("Connect");
    disable_when_run.push_back(connect_button);
    connect(connect_button, SIGNAL(clicked()), this, SLOT(toggleConnection()));

    // Create serial input window and its trigger button
    //serial_button = new QPushButton("Serial Input");
    //serial = new SerialWindow("Serial Input", this);
    //QObject::connect(serial_button, SIGNAL(clicked()), this, SLOT(serialShow()));

    // Create calibration window and its trigger buttons
    calib_button = new QPushButton("Calibration");
    disable_when_run.push_back(calib_button);
    calib = new CalibWindow("Calibration Values", this);
    QObject::connect(calib_button, SIGNAL(clicked()), this, SLOT(calibShow()));

    connect(calib, SIGNAL(calibUID(int)), this, SLOT(calibUID(int)));
    connect(calib, SIGNAL(calibLeft(int)), this, SLOT(calibLeft(int)));
    connect(calib, SIGNAL(calibRight(int)), this, SLOT(calibRight(int)));
    connect(calib, SIGNAL(calibStraight(int)), this, SLOT(calibStraight(int)));
    connect(calib, SIGNAL(calibStop()), this, SLOT(calibStop()));
    connect(calib, SIGNAL(calibSave()), this, SLOT(calibSave()));

    LED_toggle_button = new QPushButton("Dispatcher LED Toggle");
    QObject::connect(LED_toggle_button, SIGNAL(clicked()), this, SLOT(dispatcherLEDToggle()));

    QGridLayout *main_grid_layout = new QGridLayout; // layout that contains the whole window
    main_grid_layout->addWidget(createDeviceSelect(),     0,0);
    main_grid_layout->addWidget(createConfigSelect(),     0,1);
    main_grid_layout->addWidget(createKilobotFileInput(), 1,0);
    main_grid_layout->addWidget(createKilobotCommands(),  2,0);
    main_grid_layout->addWidget(createKilogridFileInput(),1,1);
    main_grid_layout->addWidget(createKilogridCommands(), 2,1);
    main_grid_layout->addWidget(createDebugWindow(), 0, 3, 3, 3); // the debug window occupies the modules 3,0 to 3,3 in the grid layout
    main_grid_layout->addWidget(status, 4, 0);

    setLayout(main_grid_layout);

    setWindowTitle("Kilobots Toolkit");
    setWindowIcon(QIcon(":/images/kilogui.png"));
    setWindowState(Qt::WindowActive);

//    vusb_conn = new VUSBConnection();
    ftdi_conn = new FTDIConnection();
//    serial_conn = new SerialConnection();

    /*
    connect(vusb_conn, SIGNAL(error(QString)), this, SLOT(showError(QString)));
    connect(vusb_conn, SIGNAL(status(QString)), this, SLOT(vusbUpdateStatus(QString)));
    */
    connect(ftdi_conn, SIGNAL(error(QString)), this, SLOT(showError(QString)));
    connect(ftdi_conn, SIGNAL(status(QString)), this, SLOT(ftdiUpdateStatus(QString)));
    connect(ftdi_conn, SIGNAL(process_trace(QString)), this, SLOT(addTextDebugWindow(QString)));
    connect(ftdi_conn, SIGNAL(requestModuleBootPage(quint8)), this, SLOT(sendModuleBootPage(quint8)));
    connect(ftdi_conn, SIGNAL(requestKilobotBootPage(quint8)), this, SLOT(sendKilobotBootPage(quint8)));
    connect(ftdi_conn, SIGNAL(canSendCommand()), this, SLOT(sendBufferedCommand()));
    connect(ftdi_conn, SIGNAL(received_text(QString)), this, SLOT(addTextDebugWindow(QString)));
/*
    connect(serial_conn, SIGNAL(error(QString)), this, SLOT(showError(QString)));
    connect(serial_conn, SIGNAL(status(QString)), this, SLOT(serialUpdateStatus(QString)));
*/
    // connect ftdi and serial objects signals to add text slots
    //connect(serial_conn, SIGNAL(readText(QString)), this, SLOT(addTextDebugWindow(QString)));

    // Create thread
    QThread *thread = new QThread();
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    // Move connections to thread
//    vusb_conn->moveToThread(thread);
    ftdi_conn->moveToThread(thread);
//    serial_conn->moveToThread(thread);

#if 0
    QDir working_dir;
    if(working_dir.exists("arena_big")) {
        baudrate = 256000;
        radio2->setChecked(true);
    }
    else {
        baudrate = 38400;
        radio1->setChecked(true);
    }
#endif
    if(app_info->hasBaudRate()) {
        // If the file Common/kilogui-dispatcher/serial_speed.h is available,
        // it is not required to keep the baud rate in KiloGUI_Info.
        baudrate = app_info->getLastBaudRateUsed();
    } else {
        // [DEFAULT] determined from Common/kilogui-dispatcher/serial_speed.h
        baudrate = DISPATCHER_BAUDRATE;
    }
    switch(baudrate) {
        case SMALL_ARENA_BAUDRATE:
            radio1->click();
            break;
        case LARGE_ARENA_BAUDRATE:
            radio2->click();
            break;
    }

    // Start thread and open connections
    thread->start();
//    vusb_conn->open();
//    serial_conn->open();

    exp_running = false;
    kilo_program = NULL;
    module_program = NULL;

    if(app_info->isAvailable(KiloGUI_Info::CONFIGURATION_FILE)) {
        setConfigurationFile(app_info->getLast(KiloGUI_Info::CONFIGURATION_FILE));
    }

    QByteArray packet(3,0);
    packet[0] = SERIAL_PACKET_HEADER;
    packet[1] = SERIAL_PACKET_DISPATCHER_CONTINUE;
    packet[2] = packet[0] ^ packet[1];

    continue_command = packet;
    command_buffer = continue_command;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    // launch ftdi thread for reading input serial data
    //if (device == 0)
        //ftdi_conn->read();
    //if (device == 2)
        //serial_conn->read();
    is_initialized = true;
}

void KiloWindow::closeEvent(QCloseEvent *event){
//    QMessageBox::StandardButton resBtn = QMessageBox::question( this, APP_NAME,
//                                                                tr("Are you sure?\n"),
//                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
//                                                                QMessageBox::Yes);
//    if (resBtn != QMessageBox::Yes) {
//        event->ignore();
//    } else {    
        saveDebugWindowToLogFile(); // save the work to the log file
        event->accept();
        //    }
    if(module_program != NULL) delete module_program;
    if(kilo_program != NULL) delete kilo_program;
    delete app_info;
}

QGroupBox *KiloWindow::createDeviceSelect() {
    QGroupBox *box = new QGroupBox("Select FTDI speed");
    QHBoxLayout *hbox = new QHBoxLayout;
    //QRadioButton *ftdi_button = new QRadioButton("FTDI");
    // QRadioButton *vusb_button = new QRadioButton("VUSB");
    //QRadioButton *serial_button = new QRadioButton("Serial");

    //QObject::connect(ftdi_button, SIGNAL(clicked()), this, SLOT(selectFTDI()));
    // QObject::connect(vusb_button, SIGNAL(clicked()), this, SLOT(selectVUSB()));
    //QObject::connect(serial_button, SIGNAL(clicked()), this, SLOT(selectSerial()));

    //hbox->addWidget(ftdi_button);
    // hbox->addWidget(vusb_button);
    //hbox->addWidget(serial_button);
    /*
    combo = new QComboBox();
    combo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
    QObject::connect(combo, SIGNAL(activated(int)), this, SLOT(setPort(int)));
    hbox->addWidget(combo);

    combo->setEnabled(false);*/
    //ftdi_button->setChecked(true);

    // QVector<QString> ports = SerialConnection::enumerate();
    // if (ports.size() > 0) {
    //     for (int i = 0; i < ports.size(); i++)
    //         combo->addItem(QString("%1. %2").arg(i+1).arg(ports[i]), ports[i]);
    // }

    radio1 = new QRadioButton(tr("Small arena, OHC"));
    radio2 = new QRadioButton(tr("Big arena"));

    disable_when_run.push_back(radio1);
    disable_when_run.push_back(radio2);

    hbox->addWidget(radio1);
    hbox->addWidget(radio2);
    box->setLayout(hbox);

    QSignalMapper *mapper = new QSignalMapper(this);
    mapper->setMapping(radio1, 0);
    mapper->setMapping(radio2, 1);
    QObject::connect(mapper, SIGNAL(mapped(int)), this, SLOT(serialSpeedChanged(int)));
    QObject::connect(radio1, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    QObject::connect(radio2, SIGNAL(clicked(bool)), mapper, SLOT(map()));

    box->setLayout(hbox);

    return box;
}

QGroupBox *KiloWindow::createConfigSelect() {
    QGroupBox *box = new QGroupBox("Select configuration");
    QGridLayout *layout = new QGridLayout;

    chooseConfigButton = new QPushButton("[select file]");
    disable_when_run.push_back(chooseConfigButton);
    QLabel *configuration_label = new QLabel("Configuration:");
    configuration_label->setBuddy(chooseConfigButton);
    QObject::connect(chooseConfigButton, SIGNAL(clicked()), this, SLOT(chooseConfigurationFile()));

    layout->addWidget(configuration_label, 0, 0);
    layout->addWidget(chooseConfigButton, 0, 1);

    box->setLayout(layout);

    return box;
}

QGroupBox *KiloWindow::createKilobotFileInput() {
    QSignalMapper *mapper = new QSignalMapper(this);
    QGroupBox *box = new QGroupBox("Upload Kilobot Program");
    QGridLayout *layout = new QGridLayout;

    chooseKilobotProgramButton = new QPushButton("[select file]");
    QLabel *program_label = new QLabel("Program:");
    program_label->setBuddy(chooseKilobotProgramButton);

    kilo_bootload_button = new QPushButton("Bootload");
    upload_kilobot_button = new QPushButton("Upload");

    kilo_bootload_button->setCheckable(true);
    upload_kilobot_button->setCheckable(true);
    //upload_button->setEnabled(false);

    disable_when_run.push_back(chooseKilobotProgramButton);
    disable_when_run.push_back(kilo_bootload_button);
    disable_when_run.push_back(upload_kilobot_button);

    mapper->setMapping(kilo_bootload_button, BOOT);
    QObject::connect(kilo_bootload_button, SIGNAL(clicked()), mapper, SLOT(map()));
    QObject::connect(mapper, SIGNAL(mapped(int)), this, SLOT(broadcastIRMessage(int)));
    QObject::connect(chooseKilobotProgramButton, SIGNAL(clicked()), this, SLOT(chooseKilobotProgramFile()));
    QObject::connect(upload_kilobot_button, SIGNAL(clicked()), this, SLOT(uploadKilobotProgram()));

    layout->addWidget(program_label, 0, 0);
    layout->addWidget(chooseKilobotProgramButton, 0, 1);
    layout->addWidget(kilo_bootload_button, 1, 0, 1, 2);
    layout->addWidget(upload_kilobot_button, 2, 0, 1, 2);

    toggle_kilobot_buttons.push_back(upload_kilobot_button);
    toggle_kilobot_buttons.push_back(kilo_bootload_button);

    box->setLayout(layout);

    return box;
}

QGroupBox *KiloWindow::createKilogridFileInput() {
    QSignalMapper *mapper = new QSignalMapper(this);
    QGroupBox *box = new QGroupBox("Upload Kilogrid Program");
    QGridLayout *layout = new QGridLayout;

    chooseKilogridProgramButton = new QPushButton("[select file]");
    QLabel *program_label = new QLabel("Program:");
    program_label->setBuddy(chooseKilogridProgramButton);
    QPushButton *bootload_button = new QPushButton("Bootload");
    upload_kilogrid_button = new QPushButton("Upload");

    bootload_button->setCheckable(true);
    upload_kilogrid_button->setCheckable(true);

    disable_when_run.push_back(chooseKilogridProgramButton);
    disable_when_run.push_back(upload_kilogrid_button);
    disable_when_run.push_back(bootload_button);

    mapper->setMapping(bootload_button, KILOGRID_BOOTLOAD);
    QObject::connect(bootload_button, SIGNAL(clicked()), mapper, SLOT(map()));
    QObject::connect(mapper, SIGNAL(mapped(int)), this, SLOT(sendKilogridCommand(int))); // map the bootload button to the same slot as the other Kilogrid conmmands
    QObject::connect(chooseKilogridProgramButton, SIGNAL(clicked()), this, SLOT(chooseKilogridProgramFile()));
    QObject::connect(upload_kilogrid_button, SIGNAL(clicked()), this, SLOT(uploadKilogridProgram()));

    layout->addWidget(program_label, 0, 0);
    layout->addWidget(chooseKilogridProgramButton, 0, 1);
    layout->addWidget(bootload_button, 1, 0, 1, 2);
    layout->addWidget(upload_kilogrid_button, 2, 0, 1, 2);

    // save buttons in the list
    toggle_kilogrid_buttons.push_back(upload_kilogrid_button);
    toggle_kilogrid_buttons.push_back(bootload_button);

    box->setLayout(layout);

    return box;
}

QGroupBox *KiloWindow::createKilobotCommands() {
    QGroupBox *box = new QGroupBox("Kilobot Commands");
    QSignalMapper *mapper = new QSignalMapper(this); // this mapper object is linked to the Kilobot commands buttons
    QGridLayout *layout = new QGridLayout;

    int split = 4;

    for (int i = 0; i < NUM_KILO_COMMANDS; i++) {
        QPushButton *button = new QPushButton(KILO_COMMANDS[i].name);
        button->setCheckable(true); // set all command buttons as checkable
        toggle_kilobot_buttons.push_back(button);
        disable_when_run.push_back(button);
        if (i > split) {
            layout->addWidget(button, i-split-1, 1);
        } else {
            layout->addWidget(button, i, 0);
        }
        mapper->setMapping(button, (int)KILO_COMMANDS[i].type); // maps current button with the type of command to send, as defined in the KILO_COMMANDS table. The cast to int is necessary for the mapped(int) method to be called later
        QObject::connect(button, SIGNAL(clicked()), mapper, SLOT(map())); // maps the "clicked()" signal of the current button to the "map()" method of the mapper
        // this will generate a "mapped(int)" event when the button is clicked.
    }

    layout->addWidget(calib_button, 3, 1);

    QObject::connect(mapper, SIGNAL(mapped(int)), this, SLOT(broadcastIRMessage(int)));

    box->setLayout(layout);

    return box;
}

QGroupBox *KiloWindow::createKilogridCommands() {
    QGroupBox *box = new QGroupBox("Kilogrid Commands");
    QSignalMapper *mapper = new QSignalMapper(this); // this mapper object is linked to the Kilobot commands buttons
    QGridLayout *layout = new QGridLayout;

    module_setup_button = new QPushButton("Setup");
    disable_when_run.push_back(module_setup_button);
    layout->addWidget(module_setup_button, 0, 0);
    mapper->setMapping(module_setup_button, (int)KILOGRID_SETUP);
    QObject::connect(module_setup_button, SIGNAL(clicked()), mapper, SLOT(map()));

    QWidget *exp_widget = new QWidget();
    QHBoxLayout *exp_length_layout = new QHBoxLayout;
    exp_length = new QSpinBox();
    exp_length->setMinimum(0);
    exp_length->setMaximum(180);
    exp_length->setValue(10);
    disable_when_run.push_back(exp_length);

    QLabel *exp_length_label = new QLabel();
    exp_length_label->setText("minutes");

    exp_length_layout->addWidget(exp_length);
    exp_length_layout->addWidget(exp_length_label);
    exp_widget->setLayout(exp_length_layout);
    layout->addWidget(exp_widget, 1, 0);

    module_run_button = new QPushButton("Run");
    layout->addWidget(module_run_button, 2, 0);
    mapper->setMapping(module_run_button, (int)KILOGRID_RUN_EXP);
    QObject::connect(module_run_button, SIGNAL(clicked()), mapper, SLOT(map()));

    QObject::connect(mapper, SIGNAL(mapped(int)), this, SLOT(sendKilogridCommand(int)));

    box->setLayout(layout);
    return box;
}

QGroupBox *KiloWindow::createDebugWindow(){
    QGroupBox *box = new QGroupBox("Experiment log");
    QVBoxLayout *layout = new QVBoxLayout;

    // create debug window (add to layout)
    text_edit = new QTextEdit();
    text_edit->setReadOnly(false);
    text_edit->setLineWrapMode(QTextEdit::WidgetWidth);
    text_edit->setFixedWidth(1000);

    QPushButton *clear_button = new QPushButton("Clear");

    QObject::connect(clear_button, SIGNAL(clicked()), this, SLOT(clearDebugWindow()));

    layout->addWidget(text_edit);
    layout->addWidget(clear_button);

    // set log file name from here

//    chooseKilogridProgramButton = new QPushButton("[select file]");
//    QLabel *program_label = new QLabel("Program:");
//    program_label->setBuddy(chooseKilogridProgramButton);
//    QPushButton *bootload_button = new QPushButton("Bootload");
//    upload_kilogrid_button = new QPushButton("Upload");

//    bootload_button->setCheckable(true);
//    upload_kilogrid_button->setCheckable(true);

//    mapper->setMapping(bootload_button, KILOGRID_BOOTLOAD);
//    QObject::connect(bootload_button, SIGNAL(clicked()), mapper, SLOT(map()));
//    QObject::connect(mapper, SIGNAL(mapped(int)), this, SLOT(sendKilogridCommand(int))); // map the bootload button to the same slot as the other Kilogrid conmmands
//    QObject::connect(chooseKilogridProgramButton, SIGNAL(clicked()), this, SLOT(chooseKilogridProgramFile()));
//    QObject::connect(upload_kilogrid_button, SIGNAL(clicked()), this, SLOT(uploadKilogridProgram()));

//    layout->addWidget(program_label, 0, 0);
//    layout->addWidget(chooseKilogridProgramButton, 0, 1);
//    layout->addWidget(bootload_button, 1, 0, 1, 2);
//    layout->addWidget(upload_kilogrid_button, 2, 0, 1, 2);

//    // save buttons in the list
//    toggle_kilogrid_buttons.push_back(upload_kilogrid_button);
//    toggle_kilogrid_buttons.push_back(bootload_button);

    box->setMinimumWidth(450);
    box->setLayout(layout);

    return box;
}


void KiloWindow::setPort(int idx) {
    serial_conn->setPort(((QComboBox*)sender())->itemData(idx).toString());
}

void KiloWindow::calibShow() {
    calib->show();
}

void KiloWindow::showError(QString str) {
    QMessageBox::critical(this, "Kilobots Toolkit", str);
}

void KiloWindow::ftdiUpdateStatus(QString str) {
    ftdi_status = str;
    updateStatus();
}

void KiloWindow::vusbUpdateStatus(QString str) {
    vusb_status = str;
    updateStatus();
}

void KiloWindow::serialSpeedChanged(int source) {
    switch(source) {
    case 0:
        baudrate = 38400;
        break;

    case 1:
        baudrate = 256000;
        break;
    }

    ftdi_conn->close();
    ftdi_conn->open(baudrate);

    app_info->setCurrentBaudRate(baudrate);
    qDebug() << "Set current baud rate:" << baudrate;
}

void KiloWindow::serialUpdateStatus(QString str) {
    serial_status = str;
    updateStatus();
}

void KiloWindow::updateStatus() {
    QString str = device == 0 ? ftdi_status : (device == 1 ? vusb_status : serial_status);
    QList<QPushButton*>::iterator i;
    status->showMessage(str);
    status->setToolTip(str);
    if (str.startsWith("Connect")) { // check if the connection with the dispatcher was established
        for (i = toggle_kilobot_buttons.begin(); i != toggle_kilobot_buttons.end(); i++) { // enable kilobot buttons when dispatcher is connected
            (*i)->setEnabled(true);
        }

        for (i = toggle_kilogrid_buttons.begin(); i != toggle_kilogrid_buttons.end(); i++) { // enable kilobot buttons when dispatcher is connected
            (*i)->setEnabled(true);
        }

        chooseKilobotProgramButton->setEnabled(true);
        chooseKilogridProgramButton->setEnabled(true);
        chooseConfigButton->setEnabled(true);

        module_setup_button->setEnabled(true);
        module_run_button->setEnabled(true);
        exp_length->setEnabled(true);
        //serial_button->setEnabled(true);
        calib_button->setEnabled(true);
//        if (kilobot_program_file.isEmpty())
        if(!app_info->isAvailable(KiloGUI_Info::KILOBOT_PROGRAM))
            upload_kilobot_button->setEnabled(false);
//        if (kilogrid_program_file.isEmpty())
        if(!app_info->isAvailable(KiloGUI_Info::MODULE_PROGRAM))
            upload_kilogrid_button->setEnabled(false);
        connected = true;
        connect_button->setText("Disconnect");
    } else {
        // disable kilobot commands buttons
        for (i = toggle_kilobot_buttons.begin(); i != toggle_kilobot_buttons.end(); i++) {
            (*i)->setEnabled(false);
        }

        for (i = toggle_kilogrid_buttons.begin(); i != toggle_kilogrid_buttons.end(); i++) { // enable kilobot buttons when dispatcher is connected
            (*i)->setEnabled(false);
        }

        chooseKilobotProgramButton->setEnabled(false);
        chooseKilogridProgramButton->setEnabled(false);
        chooseConfigButton->setEnabled(false);

        // disable kilogrid commands buttons
        module_setup_button->setEnabled(false);
        module_run_button->setEnabled(false);
        exp_length->setEnabled(false);
        connected = false;
        //serial_button->setEnabled(false);
        calib_button->setEnabled(false);
        connect_button->setText("Connect");
    }
}

void KiloWindow::toggleConnection() {
    if (device == 0) {
        if (ftdi_status.startsWith("Connect"))
            ftdi_conn->close();
        else
            ftdi_conn->open(baudrate);
    } else if (device == 1) {
        if (vusb_status.startsWith("Connect"))
            vusb_conn->close();
        else
            vusb_conn->open();
    } else {
        if (serial_status.startsWith("Connect"))
            serial_conn->close();
        else
            serial_conn->open();
    }
}

void KiloWindow::selectFTDI() {
    device = 0;
    combo->setEnabled(false);
    updateStatus();
}

void KiloWindow::selectVUSB() {
    device = 1;
    combo->setEnabled(false);
    updateStatus();
}

void KiloWindow::selectSerial() {
    device = 2;
    updateStatus();

     QVector<QString> ports = SerialConnection::enumerate();
     combo->clear();
     if (ports.size() > 0) {
         for (int i = 0; i < ports.size(); i++)
             combo->addItem(QString("%1. %2").arg(i+1).arg(ports[i]), ports[i]);
     }
     combo->setEnabled(true);
}

void KiloWindow::uploadKilobotProgram() {

    //kilogrid_disable_IR();
    const auto target = KiloGUI_Info::KILOBOT_PROGRAM;
    upload_kilobot_button->setChecked(true); // tells the user that KiloGUI is uploading the program

    if (!app_info->isAvailable(target)) {
         QMessageBox::critical(this, "Kilobots Toolkit", "You must select a proper program file to upload.");
    }
    else {
        if(kilo_program != NULL) delete kilo_program;
        kilo_program = new ProgramStore(app_info->getLast(target));
        kilogrid_IR_enabled = true;

        qDebug() << "Upload kilobot program:" << app_info->getLast(target);

        ftdi_conn->startKilobotBootload(kilo_program->getNumBootPages());
        ftdi_conn->read();
    }
}

void KiloWindow::uploadKilogridProgram() {

    //kilogrid_disable_command();
    const auto target = KiloGUI_Info::MODULE_PROGRAM;
    upload_kilogrid_button->setChecked(true); // tells the user that KiloGUI is uploading the program

    if (!app_info->isAvailable(target)) {
         QMessageBox::critical(this, "Kilobots Toolkit", "You must select a proper program file to upload.");
    }
    else {
        if(module_program != NULL) delete module_program;
        module_program = new ProgramStore(app_info->getLast(target));
        kilogrid_command_active = true;

        qDebug() << "Upload module program:" << app_info->getLast(target);

//        if (device == 0)
        ftdi_conn->startModuleBootload(module_program->getNumBootPages());
        ftdi_conn->read();
    }
}

void KiloWindow::sendModuleBootPage(quint8 bootpage_num) {
    ftdi_conn->sendModuleBootpage(module_program->getBootPage(bootpage_num), bootpage_num);
    ftdi_conn->read();

    if(bootpage_num == module_program->getNumBootPages()-1) {
        for(int i=0; i<toggle_kilogrid_buttons.size(); i++) {
            toggle_kilogrid_buttons.at(i)->setChecked(false);
            kilogrid_command_active = false;
        }
    }
}

void KiloWindow::sendKilobotBootPage(quint8 bootpage_num) {
    if(kilo_bootload_button->isChecked()) {
        ftdi_conn->sendKilobotBootpage(kilo_program->getBootPage(bootpage_num), bootpage_num);
        ftdi_conn->read();
    }
}

void KiloWindow::changeKilogridProgramFile(QString filename) {
    const auto target = KiloGUI_Info::MODULE_PROGRAM;

    if (filename.isEmpty()) {
        qDebug() << "Module program was not changed.";
        return;
    }
    app_info->setCurrent(target, filename);
    upload_kilogrid_button->setEnabled(false);

    if (app_info->isAvailable(target)) {
        QString path = app_info->getLast(target);
        chooseKilogridProgramButton->setText(QFileInfo(path).fileName());
        upload_kilogrid_button->setEnabled(connected);
        qDebug() << "Set module program:" << path;
    } else {
        QMessageBox::critical(this, "Kilobots Toolkit", "Unable to open program file for reading.");
        chooseKilogridProgramButton->setText("[select file]");
    }
}

void KiloWindow::chooseKilogridProgramFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Program File", app_info->getWorkingDirectoryPath(), "Program Hex File (*.hex)"); //launches File Selector
    this->changeKilogridProgramFile(filename);
}

void KiloWindow::changeKilobotProgramFile(QString filename)
{
    const auto target = KiloGUI_Info::KILOBOT_PROGRAM;

    if (filename.isEmpty()) {
        qDebug() << "Kilobot program was not changed.";
        return;
    }
    app_info->setCurrent(target, filename);
    upload_kilobot_button->setEnabled(false);

    if(app_info->isAvailable(target)) {
        QString path = app_info->getLast(target);
        chooseKilobotProgramButton->setText(QFileInfo(path).fileName());
        upload_kilobot_button->setEnabled(connected);
        qDebug() << "Set kilobot program:" << path;
    } else {
        QMessageBox::critical(this, "Kilobots Toolkit", "Unable to open program file for reading.");
        chooseKilobotProgramButton->setText("[select file]");
    }
}

void KiloWindow::chooseKilobotProgramFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Program File", app_info->getWorkingDirectoryPath(), "Program Hex File (*.hex)"); //launches File Selector
    this->changeKilobotProgramFile(filename);
}

void KiloWindow::chooseConfigurationFile() {
    // Open dialog to choose new configuration file
    // The directory shown is that of the previous configuration file
    QString filename = QFileDialog::getOpenFileName(this, "Open configuration file", app_info->getWorkingDirectoryPath(), "Kilogrid configuration file (*.kconf)"); //launches File Selector
    setConfigurationFile(filename);
}

void KiloWindow::setConfigurationFile(QString filename)
{
    const auto target = KiloGUI_Info::CONFIGURATION_FILE;

    if (filename.isEmpty()) {
        qDebug() << "Configuration file was not changed.";
        return;
    }
    app_info->setCurrent(target, filename);

    if(app_info->isAvailable(target)) {
        QString path = app_info->getLast(target);
        chooseConfigButton->setText(QFileInfo(path).fileName());

        qDebug() << "Set working directory:" << app_info->getWorkingDirectoryPath();
        qDebug() << "Set configuration file:" << app_info->getLast(target);

        if(app_info->isAvailable(KiloGUI_Info::MODULE_PROGRAM)) {
            this->changeKilogridProgramFile(app_info->getLast(KiloGUI_Info::MODULE_PROGRAM));
        } else {
            chooseKilogridProgramButton->setText("[select file]");
            upload_kilogrid_button->setEnabled(false);
        }
        if(app_info->isAvailable(KiloGUI_Info::KILOBOT_PROGRAM)) {
            this->changeKilobotProgramFile(app_info->getLast(KiloGUI_Info::KILOBOT_PROGRAM));
        } else {
            chooseKilobotProgramButton->setText("[select file]");
            upload_kilobot_button->setEnabled(false);
        }
    } else {
        QMessageBox::critical(this, "Kilobots Toolkit", "Unable to open configuration file for reading.");
        chooseConfigButton->setText("[select file]");
    }
}

void KiloWindow::kilogrid_disable_IR() {
    if (kilogrid_IR_enabled)
        sendKilogridCommand(KILOGRID_STOP_IR);

    kilogrid_IR_enabled = false;

    // toggle buttons state
    QList<QPushButton*>::iterator i;
    for (i = toggle_kilobot_buttons.begin(); i != toggle_kilobot_buttons.end(); i++) {
        if ((*i)->isChecked())
            (*i)->setChecked(false); // uncheck all checked buttons if this method is called
    }
}

// This function is called when a Kilogrid command button is unchecked
void KiloWindow::kilogrid_disable_command(){

    kilogrid_command_active = false;

    sendKilogridCommand(KILOGRID_IDLE); // IDLE command puts the module in IDLE state (toggle between active commands and this state)

    // toggle buttons state
    QList<QPushButton*>::iterator i;
    for (i = toggle_kilogrid_buttons.begin(); i != toggle_kilogrid_buttons.end(); i++) {
        if ((*i)->isChecked())
            (*i)->setChecked(false); // uncheck all checked buttons if this method is called
    }
    if(exp_running) {
        module_run_button->setText("Wait for it...");
        module_run_button->setEnabled(false);
        timer->stop();
    }
}

void KiloWindow::sendKilogridConfiguration() {
    const auto target = KiloGUI_Info::CONFIGURATION_FILE;

    if(!app_info->isAvailable(target)) {
        return;
    }

    ConfigurationParser p(app_info->getLast(target));
    QList<ConfigurationItem> items;

    items = p.parse();

    // send out all items
    for(int i=0; i<items.size(); i++) {
        ConfigurationItem item = items.at(i);

        // prepare the configuration message
        QByteArray packet(SERIAL_PACKET_SIZE, 0);
        packet[0] = SERIAL_PACKET_HEADER;
        packet[1] = SERIAL_PACKET_MODULE_CONFIGURATION;
        packet[5] = item.size;

        uint8_t checksum = packet[0] ^ packet[1] ^ packet[5];

        for(int k=0; k<item.size; k++) {
            packet[6+k] = item.data[k];
            checksum = checksum ^ packet[6+k];
        }

        // send out data for all address
        for(int j=0; j<item.addresses.size(); j++) {
            kilogrid_address_t address = item.addresses.at(j);

            packet[2] = address.x;
            packet[3] = address.y;
            packet[4] = address.type;

            uint8_t final_checksum = checksum ^ packet[2] ^ packet[3] ^ packet[4];
            packet[SERIAL_PACKET_SIZE - 1] = final_checksum;

            sendSerialPacket(packet);

            delay_no_freeze(100);
        }
    }
}

/*
 * This function requires a single int as an argument (linked to the "mapper(int)" signal
 * in the createCommands() method above.
*/
void KiloWindow::broadcastIRMessage(int type_int) {
    unsigned char type = (unsigned char)type_int;
    QByteArray packet(SERIAL_PACKET_SIZE, 0);

    if (kilogrid_IR_enabled) {
        kilogrid_disable_IR();
        return;
    }

    kilogrid_IR_enabled = true;
    packet[0] = SERIAL_PACKET_HEADER;
    packet[1] = SERIAL_PACKET_KILO_FORWARD_IR_MSG;
    packet[11] = type;
    packet[SERIAL_PACKET_SIZE-1] = SERIAL_PACKET_HEADER^SERIAL_PACKET_KILO_FORWARD_IR_MSG^type;

    if(exp_running) {
        command_buffer = packet;
    }
    else {
        sendSerialPacket(packet);
    }
}

void KiloWindow::sendKilogridCommand(int type_int){
    unsigned char type = (unsigned char)type_int; // cast type into an unsigned char (keep the first bits)
    QByteArray packet(SERIAL_PACKET_SIZE, 0);

    if(kilogrid_command_active){
        // disable any commands to go back to idle mode
        kilogrid_disable_command();

        // save debug window to log file if the experiment was running
        if(exp_running){
            exp_running = false;
        }

        return;
    }

    if(type == KILOGRID_SETUP) {
        onExperimentStarted();
        module_run_button->setEnabled(false);
        module_setup_button->setText("Sending...");
        sendKilogridConfiguration();
        module_setup_button->setText("Setup");
        onExperimentStopped();
        module_setup_button->toggle();
    }
    
    packet[0] = SERIAL_PACKET_HEADER;
    packet[1] = SERIAL_PACKET_KILOGRID_COMMAND;
    packet[2] = type;
    packet[SERIAL_PACKET_SIZE-1] = packet[0] ^ packet[1] ^ packet[2];

    if(type != KILOGRID_STOP_IR && type != KILOGRID_IDLE && type != KILOGRID_SETUP) {
        kilogrid_command_active = true; // necessary to keep the logic of the kilogrid_command_active button
    }

    if(exp_running) {
        command_buffer = packet;
    }
    else if(type == KILOGRID_RUN_EXP) {

        if(radio1->isChecked()){
            packet[3] = 4; // X dimension
            packet[4] = 4; // Y dimension
            packet[5] = 10; // threshold for the tracking items in the serial packet (dispatcher side)
        }
        else{
            packet[3] = 10; // X dimension
            packet[4] = 20; // Y dimension
            packet[5] = 30; // threshold for the tracking items in the serial packet (dispatcher side)
        }

        packet[SERIAL_PACKET_SIZE-1] = packet[SERIAL_PACKET_SIZE-1] ^ packet[3] ^ packet[4] ^ packet[5];
        sendSerialPacket(packet);
    }
    else {
        sendSerialPacket(packet);
    }

    // ftdi: go back to read mode after issuing a RUN command
    if(type == KILOGRID_RUN_EXP){
        ftdi_conn->FTDIPurgeRxBuffer();
        wait_for_last_experiment_data_packet = 0;

        QString f_name = QDateTime::currentDateTime().toString("'exp_data_'MM_dd_yyyy_hh'h'mm_ss's.kilodat'"); // extension of the file will be kilodat
//        tracking_parser = new TrackingParser(exp_directory, f_name);
        tracking_parser = new TrackingParser(app_info->getWorkingDirectoryPath(), f_name);
        addTextDebugWindow(TrackingParser::getHeader());
        connect(ftdi_conn, SIGNAL(received_packet(serial_packet_tracking_t*)), this, SLOT(onExperimentDataReceived(serial_packet_tracking_t*)));
        connect(tracking_parser, SIGNAL(onTrackingDataReceived(QString)), this, SLOT(addTextDebugWindow(QString)));

        ftdi_conn->read(); // ftdi: go back to read mode after issuing a RUN command
        command_buffer = continue_command;
        exp_running = true; // will be put to zero when disabling any command
        onExperimentStarted();
        timer->start(60000);
    }
}

void KiloWindow::onExperimentDataReceived(serial_packet_tracking_t* t) {
    tracking_parser->onExperimentDataReceived(t);

    if(wait_for_last_experiment_data_packet) {
        disconnect(ftdi_conn, SIGNAL(received_packet(serial_packet_tracking_t*)), this, SLOT(onExperimentDataReceived(serial_packet_tracking_t*)));
        disconnect(tracking_parser, SIGNAL(onTrackingDataReceived(QString)), this, SLOT(addTextDebugWindow(QString)));
        delete tracking_parser;
    }
}

void KiloWindow::sendBufferedCommand() {
    sendSerialPacket(command_buffer);

    if(command_buffer[2] == (char)KILOGRID_IDLE) {
        wait_for_last_experiment_data_packet = 1;
        onExperimentStopped();
    }

    command_buffer = continue_command;
    ftdi_conn->read();
}

void KiloWindow::onTimer() {
    exp_length->setValue(exp_length->value()-1);

    if(exp_length->value() == 0) {
        module_run_button->click();
    }
}

void KiloWindow::sendSerialPacket(QByteArray packet) {
    if (device == 0) {
        ftdi_conn->sendCommand(packet);
    }
    else if (device == 1) {
        vusb_conn->sendCommand(packet);
    }
    else {
        serial_conn->sendCommand(packet);
    }
}

void KiloWindow::dispatcherLEDToggle(){
    QByteArray packet(SERIAL_PACKET_SIZE, 0);

    packet[0] = SERIAL_PACKET_HEADER;
    packet[1] = SERIAL_PACKET_LEDTOGGLE;
    packet[SERIAL_PACKET_SIZE-1]=SERIAL_PACKET_HEADER^SERIAL_PACKET_LEDTOGGLE; // checksum

    sendSerialPacket(packet);
}

void KiloWindow::broadcastIRDataMessage(uint8_t *payload, IR_message_type_t type) {
    if (kilogrid_IR_enabled)
        kilogrid_disable_IR();

    QByteArray packet(SERIAL_PACKET_SIZE, 0);
    uint8_t checksum = SERIAL_PACKET_HEADER^SERIAL_PACKET_KILO_FORWARD_IR_MSG^type;
    packet[0] = SERIAL_PACKET_HEADER;
    packet[1] = SERIAL_PACKET_KILO_FORWARD_IR_MSG;
    for (int i = 0; i < 9; i++) {
        packet[2+i] = payload[i];
        checksum ^= payload[i];
    }
    packet[11] = type;
    packet[SERIAL_PACKET_SIZE-1] = checksum;
    kilogrid_IR_enabled = true;

    sendSerialPacket(packet);
}

void KiloWindow::calibStop() {
    if (kilogrid_IR_enabled) {
        broadcastIRMessage(WAKEUP);
        broadcastIRMessage(WAKEUP);
    }
}

void KiloWindow::calibSave() {
    static calibmsg_t msg;
    msg.mode = CALIB_SAVE;
    broadcastIRDataMessage((uint8_t*)&msg, CALIB);
}
void KiloWindow::calibUID(int x) { 
    static calibmsg_t msg;
    msg.mode = CALIB_UID;
    msg.uid = x;
    broadcastIRDataMessage((uint8_t *)&msg, CALIB);
}
void KiloWindow::calibLeft(int x) {
    static calibmsg_t msg;
    msg.mode = CALIB_TURN_LEFT;
    msg.turn_left = x;
    broadcastIRDataMessage((uint8_t *)&msg, CALIB);
}
void KiloWindow::calibRight(int x) {
    static calibmsg_t msg;
    msg.mode = CALIB_TURN_RIGHT;
    msg.turn_right = x;
    broadcastIRDataMessage((uint8_t *)&msg, CALIB);
}
void KiloWindow::calibStraight(int x) {
    static calibmsg_t msg;
    msg.mode = CALIB_STRAIGHT;
    msg.straight_left = x&0xFF;
    msg.straight_right = (x>>8) & 0xFF;
    broadcastIRDataMessage((uint8_t *)&msg, CALIB);
}

void KiloWindow::clearDebugWindow() {
    text_edit->setPlainText("");
}

void KiloWindow::addTextDebugWindow(QString text) {
    text_edit->moveCursor(QTextCursor::End);
    text_edit->insertPlainText(text);
    text_edit->moveCursor(QTextCursor::End);
    text_edit->ensureCursorVisible();
}

void KiloWindow::saveDebugWindowToLogFile(){
    if(!app_info->getWorkingDirectoryPath().isEmpty() && !text_edit->toPlainText().isEmpty()){
        // creating filename containing the date and time of the end of the experiment
        QString f_name = QDateTime::currentDateTime().toString("'log_exp_'MM_dd_yyyy_hh'h'mm.ss's.txt'");

        // creating subfolder "log" if need be
        //QDir logdir = QDir(exp_directory);
        QDir logdir = QDir(app_info->getWorkingDirectoryPath());
        logdir.mkdir("log");
        logdir.cd("log");

        // try writing to text file
        //QMessageBox msgBox;

        tracking_parser->writeTextEditToLogFile(text_edit->toPlainText(), logdir, f_name);
        //msgBox.setText("Experiment log file saved in " + logdir.filePath(f_name));
        //msgBox.exec();
    }
}

void KiloWindow::setWidgetStates(bool state)
{
    for(auto* widget : disable_when_run) {
        widget->setEnabled(state);
    }
}

void KiloWindow::onExperimentStarted()
{
    setWidgetStates(false);
    module_run_button->setText("Stop");
}

void KiloWindow::onExperimentStopped()
{
    setWidgetStates(true);
    module_run_button->setEnabled(true);
    module_run_button->setText("Run");
}
