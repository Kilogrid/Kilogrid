#ifndef KILOWINDOW_H
#define KILOWINDOW_H

#include <QWidget>
#include <QList>
#include <stdint.h>

#ifdef _WIN32
#include "windows.h"
#endif

#include "vusbconn.h"
#include "ftdiconn.h"
#include "serialconn.h"
#include "calibrate.h"
#include <message.h>
#include <QTextEdit>
#include "trackingparser.h"
#include "mylineedit.h"
#include "kilogui_info.h"
#include "program_store.h"
#include <QRadioButton>

class QGroupBox;
class QToolButton;
class QStatusBar;
class QPushButton;
class QComboBox;

class KiloWindow: public QWidget {
    Q_OBJECT

public:
    KiloWindow(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    bool is_initialized;  // false :: in the constructor process, true :: otherwise

    int device;
    VUSBConnection *vusb_conn;
    FTDIConnection *ftdi_conn;
    SerialConnection *serial_conn;
    QString vusb_status;
    QString ftdi_status;
    QString serial_status;

    QStatusBar *status;
    QToolButton *connect_button;

    //SerialWindow *serial;
    QTextEdit *text_edit;
    CalibWindow *calib;
    KiloGUI_Info *app_info; // stores information during the use of KiloGUI, e.g. experiment folder, etc

    QGroupBox *createKilobotFileInput();
    QGroupBox *createKilogridFileInput();
    QGroupBox *createKilobotCommands();
    QGroupBox *createKilogridCommands();
    QGroupBox *createDeviceSelect();
    QGroupBox *createConfigSelect();
    QGroupBox *createDebugWindow();

    QRadioButton *radio1;
    QRadioButton *radio2;

    bool kilogrid_IR_enabled;
    bool kilogrid_command_active;
    bool connected;
    int baudrate;

    bool exp_running;
    QSpinBox *exp_length;
    QByteArray command_buffer;
    QTimer *timer;

    TrackingParser *tracking_parser;
    ProgramStore* kilo_program;
    ProgramStore* module_program;
    QByteArray continue_command;
    bool wait_for_last_experiment_data_packet;

    QPushButton *chooseKilobotProgramButton, *chooseKilogridProgramButton, *chooseConfigButton;
    QPushButton *upload_kilobot_button, *upload_kilogrid_button, *calib_button, *LED_toggle_button;
    QPushButton *kilo_bootload_button;
    QPushButton *module_setup_button, *module_run_button;
    QComboBox *combo;
    QList<QPushButton*> toggle_kilobot_buttons, toggle_kilogrid_buttons;
    QList<QWidget*> disable_when_run;
    void updateStatus();
    void sendKilogridConfiguration();
    void setConfigurationFile(QString);

private slots:
    void selectFTDI();
    void selectVUSB();
    void selectSerial();
    void toggleConnection();
    void showError(QString);
    void serialUpdateStatus(QString);
    void serialSpeedChanged(int);
    void ftdiUpdateStatus(QString);
    void vusbUpdateStatus(QString);
    void chooseKilobotProgramFile();
    void changeKilobotProgramFile(QString filename);
    void chooseKilogridProgramFile();
    void changeKilogridProgramFile(QString filename);
    void chooseConfigurationFile();
    void uploadKilobotProgram();
    void uploadKilogridProgram();
    void kilogrid_disable_IR();
    void kilogrid_disable_command();
    void broadcastIRMessage(int);
    void broadcastIRDataMessage(uint8_t *, IR_message_type_t);
    void sendKilogridCommand(int);
    void sendSerialPacket(QByteArray);
    void dispatcherLEDToggle();
    //void serialShow();
    void calibShow();
    void setPort(int);
    void onExperimentDataReceived(serial_packet_tracking_t*);

    void calibUID(int);
    void calibLeft(int);
    void calibRight(int);
    void calibStraight(int);
    void calibSave();
    void calibStop();

    void sendModuleBootPage(quint8);
    void sendKilobotBootPage(quint8);

    void onTimer();
    void sendBufferedCommand();
    void addTextDebugWindow(QString);
    void clearDebugWindow();
    void saveDebugWindowToLogFile();

private:
    void setWidgetStates(bool);
    void onExperimentStarted();
    void onExperimentStopped();
};

#endif//KILOWINDOW_H
