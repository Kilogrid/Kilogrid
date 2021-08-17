#include "kilogui_info.h"
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDateTime>

// todo: move all file dialog and experiment directory choice here

namespace {
    const QString KILOGUI_INFO_FILE_NAME    = "kilogui_info.xml";
    const QString TAG_FOR_CONFIGURATON_FILE = "last_experiment_config_file";
    const QString TAG_FOR_KILOBOT_PROGRAM   = "last_kilobot_controller_program";
    const QString TAG_FOR_MODULE_PROGRAM      = "last_module_controller_program";
    const QString TAG_FOR_BAUD_RATE         = "last_boud_rate";
}

KiloGUI_Info::KiloGUI_Info(QString info_file_dir)
    : info_file_dir_and_name(info_file_dir + "/" + KILOGUI_INFO_FILE_NAME),
      configuration_file_dir_and_name(""), kilobot_controller_path(""), module_controller_path(""), baudrate(-1)  // make default values empty
{
//    QString info_file_name = "kilogui_info.xml";
//    info_file_dir_and_name = info_file_dir + "/" + info_file_name;

    QFileInfo file(info_file_dir_and_name); // get information on the info file

//    bool file_exists = file.exists() && file.isFile(); // indicates if the file exists prior to the creation of the KiloGUI_Info object

    if(file.exists() && file.isFile()){
        readInfoFile(); // reads all information about KiloGUI contained in the file
    }
    else{
        updateInfoFile(); // creates an XML information file
    }
}

KiloGUI_Info::~KiloGUI_Info(){
    updateInfoFile();
}

void KiloGUI_Info::updateInfoFile(){

    QFile info_file(info_file_dir_and_name);

    info_file.open(QIODevice::ReadWrite | QIODevice::Truncate); // overwrite existing file

    // overwrite info file
    QXmlStreamWriter xml_writer(&info_file); // link XML writer object to file
    xml_writer.setAutoFormatting(true);

    xml_writer.writeStartDocument();
    xml_writer.writeDTD("<!DOCTYPE kilogui_info>");
    xml_writer.writeStartElement("kilogui_info");
    //xml_writer.writeAttribute("version", "1.0");

    xml_writer.writeTextElement("last_updated", QDateTime::currentDateTime().toString());

    /**** Write all attributes here ****/
    if(isAvailable(CONFIGURATION_FILE)) {
        xml_writer.writeTextElement(TAG_FOR_CONFIGURATON_FILE, configuration_file_dir_and_name);
    }
    if(isAvailable(KILOBOT_PROGRAM)) {
        xml_writer.writeTextElement(TAG_FOR_KILOBOT_PROGRAM, kilobot_controller_path);
    }
    if(isAvailable(MODULE_PROGRAM)) {
        xml_writer.writeTextElement(TAG_FOR_MODULE_PROGRAM, module_controller_path);
    }
    if(hasBaudRate()) {
        xml_writer.writeTextElement(TAG_FOR_BAUD_RATE, QString::number(baudrate));
    }
    xml_writer.writeEndDocument();

    info_file.close();
}

void KiloGUI_Info::readInfoFile(){

    QFile info_file(info_file_dir_and_name);
    info_file.open(QIODevice::ReadOnly);

    QXmlStreamReader xml_reader(&info_file);

    //xml_reader.setDevice(info_file);
    xml_reader.readNextStartElement(); // read first element of the XML file

    // look for all parameters of KiloGUI inside the information file
    while(!xml_reader.atEnd()){

        // look for the configuration file directory element in the xml file
        if(xml_reader.name() == TAG_FOR_CONFIGURATON_FILE){
            xml_reader.readNext();
            configuration_file_dir_and_name = xml_reader.text().toString(); // read configuration file path
            xml_reader.readNext(); // skip end of element mark
        } else if (xml_reader.name() == TAG_FOR_KILOBOT_PROGRAM) {
            xml_reader.readNext();
            kilobot_controller_path = xml_reader.text().toString();
            xml_reader.readNext();
        } else if (xml_reader.name() == TAG_FOR_MODULE_PROGRAM) {
            xml_reader.readNext();
            module_controller_path = xml_reader.text().toString();
            xml_reader.readNext();
        } else if (xml_reader.name() == TAG_FOR_BAUD_RATE ) {
            xml_reader.readNext();
            bool flag = false;
            baudrate = xml_reader.text().toInt(&flag, 10);
            if(!flag) baudrate = -1;
            xml_reader.readNext();
        }
        xml_reader.readNext(); // move on to the next element
    }

    info_file.close();

}

QString KiloGUI_Info::getAbsoluteFilePath(QString file) const
{
    return getWorkingDirectoryPath() + "/" + file;
}

QString KiloGUI_Info::getWorkingDirectoryPath() const
{
    return isAvailable(CONFIGURATION_FILE) ? QFileInfo(configuration_file_dir_and_name).absoluteDir().path()
                                           : QDir::homePath();
}

bool KiloGUI_Info::isAvailable(file_type type) const
{
    auto path = getLast(type);
    return !path.isEmpty() && QFileInfo(path).isReadable();
}

QString KiloGUI_Info::getLast(file_type type) const
{
    switch(type) {
    case CONFIGURATION_FILE:
        return configuration_file_dir_and_name;
    case KILOBOT_PROGRAM:
        return kilobot_controller_path;
    case MODULE_PROGRAM:
        return module_controller_path;
    }
    return "";
}

void KiloGUI_Info::setCurrent(file_type type, QString path)
{
    switch(type) {
    case CONFIGURATION_FILE:
        configuration_file_dir_and_name = path;
        kilobot_controller_path = getWorkingDirectoryPath() + "/Kilobot/build/kilo.hex";
        module_controller_path = getWorkingDirectoryPath() + "/Module/build/Module.hex";
        break;
    case KILOBOT_PROGRAM:
        kilobot_controller_path = path;
        break;
    case MODULE_PROGRAM:
        module_controller_path = path;
        break;
    }
}

// Boud rate
bool KiloGUI_Info::hasBaudRate() {
    return baudrate > 0;
}

int KiloGUI_Info::getLastBaudRateUsed() const {
    return baudrate;
}
void KiloGUI_Info::setCurrentBaudRate(int rate) {
    baudrate = rate;
}
