#ifndef __OHC_H__
#define __OHC_H__

#include <stdint.h>
#include "message.h"

// 01010101
#define PACKET_HEADER 0x55
#define PAGE_SIZE 128
#define PACKET_SIZE (PAGE_SIZE+4)
enum {
    PACKET_STOP,
    PACKET_LEDTOGGLE,
    PACKET_FORWARDMSG,
    PACKET_FORWARDRAWMSG,
    PACKET_BOOTPAGE,
    PACKET_GPSFRAME,
    PACKET_FORWARDMSGSINGLE,
};

typedef struct {
    uint16_t id;
    int16_t x;
    int16_t y;
    int8_t theta;
    uint16_t unused;
} gpsmsg_t;

#endif//__OHC_H__
