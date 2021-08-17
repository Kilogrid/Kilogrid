#include "program_store.h"
#include "bootpage.h"

ProgramStore::ProgramStore(QString path) {
    program_data.load(path.toStdString());
    num_bootpages = program_data.size()/BOOTPAGE_SIZE+1;
}

QByteArray ProgramStore::getBootPage(uint8_t num) {
    QByteArray ret(BOOTPAGE_SIZE, 0);

    intelhex::address_type program_index = BOOTPAGE_SIZE*num;
    intelhex::address_type bootpage_end = BOOTPAGE_SIZE*(num+1);
    uint32_t ret_index = 0;
    for(; program_index < bootpage_end; program_index++, ret_index++) {
        ret[ret_index] = program_data.get(program_index);
    }

    return ret;
}

uint8_t ProgramStore::getNumBootPages() {
    return num_bootpages;
}
