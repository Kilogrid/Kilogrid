/**
 * @file     Kilogrid/src/kilob/kilob_messaging.h
 * @brief    Library of function for the Kilobot robot. 
 * The kilob messaging aims to allow users to send messages via the Kilobot.
 * @details  This library provides an interface to send messages via 
 * Kilobot's infrared.
 * 
 * The initial Kilobot library (v2) provides callback functions 
 * (::kilo_message_tx and ::kilo_message_tx_success) to communicates infrared 
 * messages with other Kilobot.
 * 
 * This additional library provides two new functions (::kilob_messaging_init
 * and ::kilob_message_send) to communicates infrared messages with other
 * Kilobot but also with cells of the Kilogrid arena.
 * \n Messages are sent in an FCFS manner, retrying forever if there are
 * collisions detected by the sender.
 * \n Note that CRC is automatically computed.
 * 
 * @author   IRIDIA lab
 * @date     March 2017
 */
#ifndef __KILOB_MESSAGING_H__
#define __KILOB_MESSAGING_H__

#include <stddef.h>
#include "kilolib.h"
#include "ringbuffer.h"
#include "message.h"

/**
 * @brief Initialize messaging by assigning function callbacks to 
 * kilo_message. 
 * @details Initialize the communication by assigning callback 
 * (::kilo_message_tx and ::kilo_message_tx_success) functions provided by 
 * kilolib.h for infrared communication. 
 */
void kilob_messaging_init();

/**
 * @brief This is the public interface for the message management implemented 
 * in kilo behaviors. 
 * @details It returns a pointer to a message object that can be 
 * filled with data and type information. The message is scheduled 
 * automatically at the end of the message queue, crc is calculated before 
 * sending.
 * 
 * Example of usage:
 * @code
 * IR_message_t* msg = kilob_message_send();
 * msg->data[i] = DATA_TO_SEND;
 * msg->type = TYPE_OF_THE_MESSAGE;
 * @endcode
 * 
 */
IR_message_t* kilob_message_send();

#endif