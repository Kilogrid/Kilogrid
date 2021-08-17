#ifndef CONFIGURATIONPARSER_H
#define CONFIGURATIONPARSER_H

#include <QString>
#include <QList>
#include <QStringList>
#include <QException>
#include "configurationitem.h"

#define MIN_Y              0
#define MAX_Y              19

#define MIN_X              0
#define MAX_X              9

class ConfigurationParser
{
public:
    ConfigurationParser(QString path);
    QList<ConfigurationItem> parse();

private:
    QString path;

    QList<kilogrid_address_t> parseCoordinates(QStringList coordinate_str);

    static void checkX(uint8_t x);
    static void checkY(uint8_t y);
};

class ConfigurationParserException {
public:
    QString message;

    ConfigurationParserException(QString message);
};

#endif // CONFIGURATIONPARSER_H
