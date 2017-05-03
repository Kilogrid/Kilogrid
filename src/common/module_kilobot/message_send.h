/**
 * @file     Kilogrid/src/common/module_kilobot/message_send.h
 * @brief    This file provides sending function for IR message.
 *   
 * @author   IRIDIA lab
 * @date     May, 2017
 */

#ifndef __MESSAGE_SEND_H__
#define __MESSAGE_SEND_H__

#include "message.h"

extern volatile uint8_t tx_mask;
uint8_t message_send(const IR_message_t *);

#endif//__MESSAGE_SEND_H__
