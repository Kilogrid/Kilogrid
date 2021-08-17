#include "configurationitem.h"
#include <cstring>

ConfigurationItem::ConfigurationItem(QList<kilogrid_address_t> addresses, QByteArray data, uint8_t size) {
    this->addresses = addresses;
    this->data = data;
    this->size = size;
}
