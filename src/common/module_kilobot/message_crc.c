/**
 * @file     Kilogrid/src/common/module_kilobot/message_crc.c
 * @brief    This file computes crc for infrared message.
 *   
 * @author   IRIDIA lab
 * @date     May, 2017
 */

#include "message.h"
#include <util/crc16.h>  // for optimized crc routines

uint16_t message_crc(const IR_message_t *msg) {
    uint8_t i;
    const uint8_t *rawmsg = (const uint8_t*)msg;
    uint16_t crc = 0xFFFF;
    for (i = 0; i < sizeof(IR_message_t)-sizeof(msg->crc); i++)
        crc = _crc_ccitt_update(crc, rawmsg[i]);
    return crc;
}
