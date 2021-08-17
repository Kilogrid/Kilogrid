#ifdef _WIN32
#include "lusb0_usb.h"
#endif

#include "usb.h"
#include "ftdiconn.h"
#include "serial_packet.h"
#include <message.h>
#include "bootpage.h"
#include "CAN.h"
#include "tracking.h"
#include "trackingparser.h"
#include "kilogrid.h"
#include <string.h> // for memcpy
#include <QDebug>

static unsigned char buf[4096];
static uint8_t packet[SERIAL_PACKET_SIZE];

enum {
    MODE_NORMAL = 0,
    MODE_UPLOAD = 0x01,
    MODE_DOWNLOAD = 0x02
};

FTDIConnection::FTDIConnection(QObject *parent): QObject(parent), ftdic(NULL), mode(MODE_NORMAL) { }

void FTDIConnection::read() {
    if (!(mode&MODE_DOWNLOAD)) {
        mode |= MODE_DOWNLOAD;
        QMetaObject::invokeMethod(this, "readLoop", Qt::QueuedConnection);
    }
}

void FTDIConnection::close() {
    if (ftdic != NULL) {
        ftdi_usb_close(ftdic);
        ftdi_free(ftdic);
        ftdic = NULL;
        mode = MODE_NORMAL;
        emit status("Disconnected.");
    }
}

void FTDIConnection::open(int baudrate) {
    QString status_msg = "Connected.";
    unsigned int chipid;

    if (ftdic != NULL) {
        if (ftdi_read_chipid(ftdic, &chipid) == 0)
            return;
        else
            close();
    }

    if (ftdic == NULL) {
        ftdic = ftdi_new();
        ftdic->usb_read_timeout = 5000;
        ftdic->usb_write_timeout = 1000;
        if (ftdi_usb_open(ftdic, 0x0403, 0x6001) != 0) {
            status_msg = QString("%1").arg(ftdic->error_str);
            ftdi_free(ftdic);
            ftdic = NULL;
        } else {
            if (ftdi_set_baudrate(ftdic, baudrate) != 0) {
                status_msg = QString("%1").arg(ftdic->error_str);
                ftdi_usb_close(ftdic);
                ftdi_free(ftdic);
                ftdic = NULL;
            } else if (ftdi_setflowctrl(ftdic, SIO_DISABLE_FLOW_CTRL) != 0) {
                status_msg = QString("%1").arg(ftdic->error_str);
                ftdi_usb_close(ftdic);
                ftdi_free(ftdic);
                ftdic = NULL;
            } else if (ftdi_set_line_property(ftdic, BITS_8, STOP_BIT_1, NONE) != 0) {
                status_msg = QString("%1").arg(ftdic->error_str);
                ftdi_usb_close(ftdic);
                ftdi_free(ftdic);
                ftdic = NULL;
            }
        }
    }

    emit status(status_msg);
}

void FTDIConnection::sendCommand(QByteArray cmd) {
    mode = MODE_NORMAL;
    if (ftdic != NULL) {
        if (ftdi_write_data(ftdic, (unsigned char *)cmd.constData(), cmd.length()) != cmd.length())
            emit error(QString(ftdic->error_str));
    } else {
        emit error("cannot send command if disconnected from usb device.");
    }
}

void FTDIConnection::startModuleBootload(uint8_t pages_total) {
    startBootload(pages_total, SERIAL_PACKET_MODULE_BOOTLOAD_START);
}

void FTDIConnection::startKilobotBootload(uint8_t pages_total) {
    startBootload(pages_total, SERIAL_PACKET_KILO_BOOTLOAD_START);
}

void FTDIConnection::startBootload(uint8_t pages_total, serial_packet_type_t type) {
    memset(packet, 0, SERIAL_PACKET_SIZE); // initialize packet
    packet[0] = SERIAL_PACKET_HEADER;
    packet[1] = type;
    packet[2] = pages_total;
    packet[SERIAL_PACKET_SIZE-1] = packet[0]^packet[1]^packet[2];

    // Check if the serial packet is not transmitted correctly
    if (ftdi_write_data(ftdic, packet, SERIAL_PACKET_SIZE) != SERIAL_PACKET_SIZE) {
        mode = MODE_NORMAL;
        emit error(QString(ftdic->error_str));
    }
}

void FTDIConnection::sendModuleBootpage(QByteArray bootpage, uint8_t bootpage_num) {
    //emit received_text(QString("Bootpage request: %1\n").arg(bootpage_num));
    sendBootPage(bootpage, bootpage_num, SERIAL_PACKET_MODULE_BOOTPAGE);
}

void FTDIConnection::sendKilobotBootpage(QByteArray bootpage, uint8_t bootpage_num) {
    emit received_text(QString("Bootpage request: %1\n").arg(bootpage_num));
    sendBootPage(bootpage, bootpage_num, SERIAL_PACKET_KILO_BOOTPAGE);
}

void FTDIConnection::sendBootPage(QByteArray bootpage, uint8_t bootpage_num, serial_packet_type_t type) {
    memset(packet, 0, SERIAL_PACKET_SIZE); // initialize packet
    packet[0] = SERIAL_PACKET_HEADER;
    packet[1] = type;
    packet[2] = bootpage_num;
    uint8_t checksum = packet[0]^packet[1]^packet[2];

    // i is the byte pointer within the current bootpage to be sent
    for (int i=0; i<BOOTPAGE_SIZE; i++) {
        packet[i+3] = bootpage[i];
        checksum ^= packet[i+3];
    }
    packet[SERIAL_PACKET_SIZE-1] = checksum;

    if (ftdi_write_data(ftdic, packet, SERIAL_PACKET_SIZE) != SERIAL_PACKET_SIZE) {
        mode = MODE_NORMAL;
        emit error(QString(ftdic->error_str));
    }
}

void FTDIConnection::readLoop() {
    int offset = 0;

    if (ftdic != NULL) {
        int num = ftdi_read_data(ftdic, buf, 4096);

        while(offset < num) {
            if(buf[offset+0] == SERIAL_PACKET_HEADER) { // data serial packet received from the dispatcher
                switch(buf[offset+1]) {
                    case SERIAL_PACKET_TRACKING:
                        tracking_packet = new serial_packet_tracking_t;

                        // parse received packet into serial packet tracking data struct
                        tracking_packet->header = buf[offset+0];
                        tracking_packet->type = (serial_packet_type_t)buf[offset+1];

                        tracking_packet->number_of_items = buf[offset+2];

                        for(uint8_t i = 0; i < tracking_packet->number_of_items; i++) {
                            tracking_packet->items[i].module_address.x = buf[offset+3 + i*sizeof(tracking_item_t)];
                            tracking_packet->items[i].module_address.y = buf[offset+4 + i*sizeof(tracking_item_t)];
                            tracking_packet->items[i].module_address.type = (kilogrid_addressing_type_t)buf[offset+5 + i*sizeof(tracking_item_t)];
                            tracking_packet->items[i].c = buf[offset+6 + i*sizeof(tracking_item_t)];

                            memcpy(tracking_packet->items[i].module_data, &buf[offset+7 + i*sizeof(tracking_item_t)], TRACKING_MSG_USER_DATA);
                            // save timestamp
                            tracking_packet->items[i].timestamp = buf[offset+7+sizeof(tracking_packet->items[i].module_data) + (i*sizeof(tracking_item_t)) ];
                            tracking_packet->items[i].timestamp += buf[offset+7+sizeof(tracking_packet->items[i].module_data)+1 + (i*sizeof(tracking_item_t))] << 8;
                            tracking_packet->items[i].timestamp += buf[offset+7+sizeof(tracking_packet->items[i].module_data)+2 + (i*sizeof(tracking_item_t))] << 16;
                            tracking_packet->items[i].timestamp += buf[offset+7+sizeof(tracking_packet->items[i].module_data)+3 + (i*sizeof(tracking_item_t))] << 24;
                        }

                        offset += 4 + tracking_packet->number_of_items*sizeof(tracking_item_t);

                        //qDebug() << "Received " << num << " bytes, tracking message is " << tracking_packet->number_of_items << " items, which is " << (tracking_packet->number_of_items*sizeof(tracking_item_t) + 4) << " bytes";

                        emit received_packet(tracking_packet);
                        break;

                    case SERIAL_PACKET_MODULE_BOOTLOAD_GET_PAGE:
                        offset += 4;
                        emit requestModuleBootPage(buf[2]);
                        break;

                    case SERIAL_PACKET_KILO_BOOTLOAD_GET_PAGE:
                        offset += 4;
                        emit requestKilobotBootPage(buf[2]);
                        break;

                    case SERIAL_PACKET_DISPATCHER_AVAILABLE:
                        offset += 4;
                        emit canSendCommand();
                        break;
                }
            }
            else { // simply display buffer as text - data non formatted
                emit received_text(QByteArray((char *)buf, num));
            }
        }
    }

    if (mode&MODE_DOWNLOAD) {
        QMetaObject::invokeMethod(this, "readLoop", Qt::QueuedConnection);
    }
}

void FTDIConnection::FTDIPurgeRxBuffer(){
    if(ftdic != NULL){
        //ftdi_usb_purge_rx_buffer(ftdic); // DO NOT DO THIS - DISPATCHER NOT RESPONDING AFTER THIS!
        while(ftdi_read_data(ftdic, buf, 4096) > 0);
    }
}
