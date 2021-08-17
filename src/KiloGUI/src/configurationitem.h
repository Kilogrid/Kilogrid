#ifndef CONFIGURATIONITEM_H
#define CONFIGURATIONITEM_H

#include <QList>
#include <QByteArray>
#include <stdint.h>
#include "kilogrid.h"

#define CONFIGURATION_DATA_SIZE 48

class ConfigurationItem
{
public:
    QList<kilogrid_address_t> addresses;
    QByteArray data;
    uint8_t size;

    ConfigurationItem(QList<kilogrid_address_t> addresses, QByteArray data, uint8_t size);
};

#endif // CONFIGURATIONITEM_H
