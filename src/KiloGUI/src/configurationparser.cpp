#include "configurationparser.h"

#include <QFile>
#include <QRegExp>
#include <QString>
#include <QByteArray>

/**
  * @brief Parser state indicates the section we are currently parsing.
  * It is either address or data.
  */
typedef enum {
    ADDRESS,
    DATA,
} parser_state_t;

/**
 * @brief Line number counter is global because it is needed in parsing methods
 * to generate error messages.
 */
int lineno = 0;

ConfigurationParser::ConfigurationParser(QString path) {
    this->path = path;
}

QList<ConfigurationItem> ConfigurationParser::parse() {
    QFile file(this->path);
    if(!file.exists()) {
        throw ConfigurationParserException(
                    QString("File does not exist: %1")
                    .arg(this->path));
    }

    //first we start with the address section
    parser_state_t state = ADDRESS;

    // list for collecting the return values
    QList<ConfigurationItem> ret;

    // buffer for collection addresses from the address section
    QList<kilogrid_address_t> addresses;

    // buffer for collecting data
    QByteArray data(CONFIGURATION_DATA_SIZE, 0);

    // size of the collected data in bytes
    uint8_t data_size = 0;

    file.open(QIODevice::ReadOnly);
    foreach (QString line_raw, QString(file.readAll()).split(QRegExp("[\r\n]"), QString::SkipEmptyParts)) {
        QString line = line_raw.trimmed(); // drop whitespace
        lineno++;

        // skip empty lines and comment lines
        if(line.startsWith("#") || line.isEmpty()) {
            continue;
        }

        switch(state) {
            case ADDRESS: {
                // skip the adress section header
                if(line.startsWith("address")) {
                    continue;
                }

                // if we detect data section header, modify the parser state
                if(line.startsWith("data")) {
                    state = DATA;
                    continue;
                }

                // address specifier determines the type of address we have
                QString address_specifier = line.section(":", 0,0).trimmed();

                // list of addresses belonging to the detected specifier
                QStringList addresses_str = line.section(":", 1,1).split(",");

                if(address_specifier == "module") {
                    addresses.append(parseCoordinates(addresses_str));
                }
                else {
                    kilogrid_address_t current_addr;
                    current_addr.x = 0;
                    current_addr.y = 0;

                    uint8_t *pCoord; // pointer to the coordinate the modify

                    if(address_specifier == "y") {
                        current_addr.type = ADDR_ROW;
                        pCoord = &(current_addr.y);
                    }
                    else if(address_specifier == "x") {
                        current_addr.type = ADDR_COLUMN;
                        pCoord = &(current_addr.x);
                    }
                    else if(address_specifier == "module") {
                        current_addr.type = ADDR_INDIVIDUAL;
                    }
                    else {
                        throw ConfigurationParserException(
                                    QString("Unknown address specifier on line %1: %2")
                                    .arg(lineno)
                                    .arg(line));
                    }

                    for(int i = 0; i < addresses_str.size(); i++) {
                        QString address_str = addresses_str.at(i);

                        // range address, i.e. 1-3
                        if(address_str.indexOf("-") > -1) {
                            int start = address_str.section("-", 0, 0).toInt();
                            int end = address_str.section("-", 1, 1).toInt();

                            for(int j = start; j <= end; j++) {

                                *pCoord = j; // assign current j to the coordinate to be modified

                                addresses.append(current_addr);
                            }
                        }
                        else { // single address
                            *pCoord = address_str.toInt();
                            addresses.append(current_addr);
                        }
                    }
                }

                break;
            }

            case DATA: {
                if(line.startsWith("address")) {
                    state = ADDRESS;

                    ret.append(ConfigurationItem(addresses, data, data_size));
                    addresses = QList<kilogrid_address_t>();
                    data = QByteArray(CONFIGURATION_DATA_SIZE, 0);
                    data_size = 0;
                    continue;
                }

                bool ok;

                // base 16 always
                uint8_t byte = line.toInt(&ok, 16);
                if(!ok) {
                    throw ConfigurationParserException(
                                QString("Error on line %1: %2, skipping data")
                                .arg(lineno)
                                .arg(line));
                }
                else {
                    data[data_size++] = byte;
                }

                break;
            }
        default:
                break;
        }
    }

    // last ConfigurationItem added here
    ret.append(ConfigurationItem(addresses, data, data_size));
    file.close();

    return ret;
}

/**
 * @brief Parser for fully specified coordinates.
 * @param coordinate_str list of coordinates to be parsed
 * @return list of addresses corresponding to the coordinate_str
 */
QList<kilogrid_address_t> ConfigurationParser::parseCoordinates(QStringList coordinate_str) {
    QList<kilogrid_address_t> ret;

    for(int i = 0; i < coordinate_str.size(); i++) {
        QStringList address_str = coordinate_str.at(i).split("-"); // convert coordinate_str in a 2-char string (xy)
        if(address_str.size() != 2) {
            throw ConfigurationParserException(
                        QString("Problem with xy coordinates on line %1: %2, skipping")
                        .arg(lineno)
                        .arg(coordinate_str.at(i)));
        }

        uint8_t x = address_str.at(0).toInt();
        uint8_t y = address_str.at(1).toInt();

        ConfigurationParser::checkX(x);
        ConfigurationParser::checkY(y);

        kilogrid_address_t a;
        a.x = x;
        a.y = y;
        a.type = ADDR_INDIVIDUAL;

        ret.append(a);
    }

    return ret;
}

/**
 * @brief Throws ConfigurationParserException if the row address is out of bounds.
 * @param row
 */
void ConfigurationParser::checkX(uint8_t x) {
    if(x > MAX_X) {
        throw ConfigurationParserException(QString("Illegal x value on line %1: %2, skipping.").arg(lineno, x));
    }
}

/**
 * @brief Throws ConfigurationParserException if the column address is out of bounds.
 * @param column
 */
void ConfigurationParser::checkY(uint8_t y) {




    if(y > MAX_Y) {
        throw ConfigurationParserException(QString("Illegal y value on line %1: %2, skipping.").arg(lineno, y));
    }
}

ConfigurationParserException::ConfigurationParserException(QString message) {this->message = message;}
