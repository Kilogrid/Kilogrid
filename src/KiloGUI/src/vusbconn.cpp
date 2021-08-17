#ifdef _WIN32
#include "windows.h"
#include "lusb0_usb.h"
#endif

#include "usb.h"
#include "vusbconn.h"
#include "serial_packet.h"
#include <message.h>

static uint8_t packet[SERIAL_PACKET_SIZE];

enum {
    MODE_NORMAL,
    MODE_UPLOAD,
};

struct usb_device *usb_find_device(int idVendor, int idProduct) {
  struct usb_bus *bus = NULL;
  struct usb_device *target = NULL;
  struct usb_device *device = NULL;

  // Initialize the USB library
  usb_init();

  // Enumerate the USB device tree
  usb_find_busses();
  usb_find_devices();

  // Iterate through attached busses and devices
  bus = usb_get_busses();
  while(bus != NULL) {
     device = bus->devices;
     while(device != NULL) {
        if((device->descriptor.idVendor == idVendor) && (device->descriptor.idProduct == idProduct)) {
           target = device;
        }
        device = device->next;
     }
     bus = bus->next;
  }

  return target;
}

int usb_write_packet(struct usb_dev_handle *handle, uint8_t *data, int length) {
    uint8_t buffer[5]={0,0,0,0,0};
    int num;
    int retry = 1000;

    for (int i=0; i<length; i+=4) {
        bool sent = false;
        while (!sent) {
            num = usb_control_msg(
                    handle,
                    USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
                    i/4,
                    data[i+0] + (data[i+1] << 8), data[i+2] + (data[i+3] << 8),
                    (char *)buffer, 4,
                    50);
            if (num < 0) {
                retry--;
                if (!retry)
                    return -1;
            }
            sent = true;
            for (int j=0; j<4; j++) {
                if (data[i+j] != buffer[j])
                    sent = false;
            }
        }
    }
    num = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
            length/4,
            0, 0,
            (char *)buffer, 4, 50);
    if (num < 0)
        return -1;
    else
        return length;

}

VUSBConnection::VUSBConnection(QObject *parent): QObject(parent), handle(NULL), mode(MODE_NORMAL) { }

void VUSBConnection::open() {
    QString status_msg = "connected.";

    if (handle == NULL) {
        struct usb_device *device = usb_find_device(0x16c0, 0x05df);
        if (device != NULL) {
            handle = usb_open(device);
            if (handle == NULL) {
                status_msg = QString("disconnected: %1").arg(usb_strerror());
            }
        } else {
            status_msg = QString("device not found");
        }
    }

    emit status(status_msg);
}

void VUSBConnection::close() {
    if (handle != NULL) {
        usb_close(handle);
        handle = NULL;
        mode = MODE_NORMAL;
        emit status("disconnected.");
    }
}

void VUSBConnection::sendCommand(QByteArray cmd) {
    mode = MODE_NORMAL;
    if (handle != NULL) {
        if (usb_write_packet(handle, (unsigned char *)cmd.constData(), cmd.length()) != cmd.length())
            emit error(QString(usb_strerror()));
    } else {
        emit error("Cannot send command if disconnected from usb device.");
    }
}

void VUSBConnection::sendProgram(QString file) {
    if (handle != NULL) {
        data.load(file.toStdString());
        page_total = data.size()/BOOTPAGE_SIZE+1;
        if (page_total > 220)
            page_total = 220;
        page = page_total;
        if (mode != MODE_UPLOAD) {
            mode = MODE_UPLOAD;
            delay.start();
            QMetaObject::invokeMethod(this, "programLoop", Qt::QueuedConnection);
        }
    } else {
        emit error("Cannot upload program if disconnected from usb device.");
    }
}

void VUSBConnection::programLoop() {
    if (handle != NULL && delay.elapsed() > 111) {
        if (page >= page_total) {
            page = 0;
            memset(packet, 0, SERIAL_PACKET_SIZE);
            packet[0] = SERIAL_PACKET_HEADER;
            packet[1] = SERIAL_PACKET_KILO_FORWARD_IR_MSG;
            packet[2] = page_total;
            packet[11] = BOOTPGM_SIZE;
            packet[SERIAL_PACKET_SIZE-1] = SERIAL_PACKET_HEADER^SERIAL_PACKET_KILO_FORWARD_IR_MSG^page_total^BOOTPGM_SIZE;
            if (usb_write_packet(handle, packet, SERIAL_PACKET_SIZE) != SERIAL_PACKET_SIZE)
                emit error(QString(usb_strerror()));
        } else {
            packet[0] = SERIAL_PACKET_HEADER;
            packet[1] = SERIAL_PACKET_KILO_BOOTPAGE;
            packet[2] = page;
            uint8_t checksum = SERIAL_PACKET_HEADER^SERIAL_PACKET_KILO_BOOTPAGE^page;
            uint8_t data_byte;
            for (int i=0; i<BOOTPAGE_SIZE; i++) {
                data_byte = data.get(page*BOOTPAGE_SIZE+i);
                packet[i+3] = data_byte;
                checksum ^= data_byte;
            }
            packet[SERIAL_PACKET_SIZE-1] = checksum;
            if (usb_write_packet(handle, packet, SERIAL_PACKET_SIZE) != SERIAL_PACKET_SIZE)
                emit error(QString(usb_strerror()));
            else
                page++;
        }
        delay.start();
    }
    if (mode == MODE_UPLOAD) {
        QMetaObject::invokeMethod(this, "programLoop", Qt::QueuedConnection);
    }
}
