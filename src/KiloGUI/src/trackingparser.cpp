#include "trackingparser.h"
#include <QString>
#include <QTextEdit>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QDir>

TrackingParser::TrackingParser(QString exp_path, QString fname)
    : flush_counter(0)
{
    QDir data_dir = QDir(exp_path);
    if(data_dir.exists()){
        data_dir.mkdir("data"); //tries to create
        data_dir.cd("data"); //enter the directory

        data_file = new QFile(data_dir.filePath(fname));
        data_file->open(QIODevice::ReadWrite);
        data_stream = new QTextStream(data_file);
        *data_stream << TrackingParser::getHeader();
    }

    this->readKilobotIDs(exp_path);
}

QString TrackingParser::getHeader() {
    return QString("modulex\tmoduley\tc\ttimestamp\tkilobot_sn\tdata0\tdata1\tdata2\tdata3\tdata4\tdata5\tdata6\n");
}

TrackingParser::~TrackingParser() {
    delete data_stream;
    delete data_file;
}

bool TrackingParser::writeTextEditToLogFile(QString data, QDir dir, QString filename) {
    QString filePath = dir.filePath(filename);
    QFile file(filePath);

    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << data;
        file.close();
        return true;
    }
    else {
        return false;
    }
}

void TrackingParser::onExperimentDataReceived(serial_packet_tracking_t *tracking_packet) {
    QByteArray tracking_packet_qba;
    tracking_packet_qba.resize(tracking_packet->number_of_items * sizeof(tracking_item_t));

    for(int i=0; i<tracking_packet->number_of_items; i++) {
        tracking_item_t item = tracking_packet->items[i];
        kilogrid_address_t address = item.module_address;

        uint32_t seconds = item.timestamp/32;
        uint8_t kilo_uid = item.module_data[0] & 0b01111111;
        QString kilobot_sn = robot_id_to_serial_no[kilo_uid];

        QString format = QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\t%9\t%10\t%11\t%12\n")
                .arg(address.x).arg(address.y).arg(item.c).arg(seconds).arg(kilobot_sn);
        QString displayString = format;
        QString dataString = format;
        QChar fillchar = '0';
        for(int j=0; j<TRACKING_MSG_USER_DATA; j++) {
            displayString = displayString.arg(item.module_data[j], 2, 16, fillchar);
            dataString = dataString.arg(item.module_data[j]);
        }

        *data_stream << dataString;
        emit onTrackingDataReceived(displayString);
        flush_counter++;
    }

    delete tracking_packet;

    if(flush_counter >= FILE_FLUSH_TRIGGER){
        data_stream->flush();
        flush_counter = 0;
    }
}

void TrackingParser::readKilobotIDs(QString exp_path) {
    QDir dir = QDir(exp_path);
    dir.cdUp();

    QFile file(dir.filePath("kilobot_ids"));
    if(!file.exists()) {
        throw TrackingParserException(QString("File does not exist: %1").arg("kilobot_ids"));
    }

    file.open(QIODevice::ReadOnly);
    foreach (QString line_raw, QString(file.readAll()).split(QRegExp("[\r\n]"), QString::SkipEmptyParts)) {
        QString line = line_raw.trimmed(); // drop whitespace

        QString serial_no = line.section("\t", 0, 0).trimmed();
        uint8_t robot_id = line.section("\t", 1, 1).toInt();

        robot_id_to_serial_no[robot_id] = serial_no;
    }
    file.close();
}

TrackingParserException::TrackingParserException(QString message) {this->message = message;}
