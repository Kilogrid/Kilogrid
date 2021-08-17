#ifndef __TRACKINGPARSER_H__
#define __TRACKINGPARSER_H__

#include <QString>
#include <QTextEdit>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QHash>
#include "tracking.h"

#define FILE_FLUSH_TRIGGER                  1 // number of calls to addExperimentDataToFile after which the file is flushed

class TrackingParser: public QObject {
    Q_OBJECT

public:
    TrackingParser(QString exp_dir, QString fname);
    ~TrackingParser();

    bool writeTextEditToLogFile(QString data, QDir dir, QString filename);
    static QString getHeader();

public slots:
    void onExperimentDataReceived(serial_packet_tracking_t*);

signals:
    void onTrackingDataReceived(QString);

private:
    QFile* data_file;
    QTextStream* data_stream;
    QHash<uint8_t, QString> robot_id_to_serial_no;
    uint8_t flush_counter;

    void readKilobotIDs(QString exp_path);
};

class TrackingParserException {
public:
    QString message;
    TrackingParserException(QString message);
};

#endif // __TRACKINGPARSER_H__
