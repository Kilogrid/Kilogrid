/** \file bootldr.h */

#ifndef BOOTLDR_H_
#define BOOTLDR_H_

#include "bootpage.h"
#include <stdint.h>

typedef struct {
    uint8_t page_address;
    uint8_t page_offset;
    uint16_t word1;
    uint16_t word2;
    uint16_t word3;
    uint8_t unused;
} bootmsg_t;

typedef struct{
	uint8_t data[BOOTPAGE_SIZE];
} bootpage_data_bytes_t;

#endif