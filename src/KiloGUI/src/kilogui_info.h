#ifndef KILOGUI_INFO_H
#define KILOGUI_INFO_H

#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>


class KiloGUI_Info
{
public:
    enum file_type {
        CONFIGURATION_FILE = 0,
        KILOBOT_PROGRAM,
        MODULE_PROGRAM
    };

    explicit KiloGUI_Info(QString);
    ~KiloGUI_Info();

    // It returns a path to the directory where configuration file is
    QString getWorkingDirectoryPath() const;

    bool isAvailable(file_type) const;
    QString getLast(file_type) const;
    void setCurrent(file_type, QString);

    // Boud rate
    bool hasBaudRate();
    int getLastBaudRateUsed() const;
    void setCurrentBaudRate(int);


private:
    void createInfoFile(QString);
    void readInfoFile();
    void updateInfoFile();

    QString getAbsoluteFilePath(QString) const;

    QString info_file_dir_and_name;           // path to info file
    QString configuration_file_dir_and_name;  // path to configuration file
    QString kilobot_controller_path;          // path to kilobot controller
    QString module_controller_path;             // path to cell controller
    int     baudrate;                         // baudrate

};

#endif // KILOGUI_INFO_H
