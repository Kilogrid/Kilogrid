#ifndef __FTDICONN_H__
#define __FTDICONN_H__

#include <ftdi.h>
#include <QObject>
#include <QString>
#include <QTime>
#include <tracking.h>

class FTDIConnection: public QObject {
    Q_OBJECT

private:
    struct ftdi_context *ftdic;
    QTime delay;
    int mode;
    int page;
    int page_total;
    serial_packet_tracking_t* tracking_packet;


public:
    FTDIConnection(QObject *p=0);
    void FTDIPurgeRxBuffer();

signals:
    void received_packet(serial_packet_tracking_t*);
    void received_text(QString);
    void status(QString);
    void process_trace(QString);
    void error(QString);
    void requestModuleBootPage(quint8);
    void requestKilobotBootPage(quint8);
    void canSendCommand();

public slots:
    void open(int baudrate);
    void close();
    void read();

private slots:
    void readLoop();

public:
    void sendCommand(QByteArray);
    void startModuleBootload(uint8_t);
    void startKilobotBootload(uint8_t);
    void sendModuleBootpage(QByteArray, uint8_t);
    void sendKilobotBootpage(QByteArray, uint8_t);

private:
    void startBootload(uint8_t pages_total, serial_packet_type_t type);
    void sendBootPage(QByteArray bootpage, uint8_t bootpage_num, serial_packet_type_t type);
};

#endif//__FTDICONN_H__
