#ifndef PROGRAM_STORE_H
#define PROGRAM_STORE_H

#include "intelhex.h"
#include "QByteArray"
#include "QString"

class ProgramStore {
private:
    intelhex::hex_data program_data;
    uint8_t num_bootpages;

public:
    ProgramStore(QString path);
    QByteArray getBootPage(uint8_t num);
    uint8_t getNumBootPages();
};

#endif // PROGRAM_STORE_H

