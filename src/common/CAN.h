/**
 * @file     Kilogrid/src/common/CAN.h
 * @brief    This file provides setup/read/write functions for CAN messaging.
 *
 * @details    Modules and the dispatcher communicates via a CAN network.
 *   Each module has an adress that can be targeted for transmiting message or
 *   received for localization of a received message.
 *   A limited version of CAN messages can be transmited (i.e. CAN_message_type_t).
 *   
 * @author   IRIDIA lab
 * @date     May, 2017
 */

#ifndef __CAN_H__
#define __CAN_H__

/**** INCLUDES ****/

#include "platform.h" // include this file for platform-specific compilation
#include "stdint.h"
#include "mcp2515.h"
#include "kilogrid.h"

/**** DEFINES ****/

//#define CAN_INTERRUPT_DISABLE // disables the use of interrupts for the CAN

/**** TYPEDEFS AND ENUMS ****/

/**
 *  @brief CAN message types enumeration. New types must be added at the end of the enumeration, for backward compatibility of the code with previous revisions, and more importantly with the module bootloader firmware.
 *  Maximum 2^6 = 64 types are alloved, because serialize_tracking_message uses the highest 2 bits for the module identifier.
 */
typedef enum{
    CAN_MODULE_USER = 0x00,
    CAN_FORWARD_IR_MESSAGE,
    CAN_FORWARD_IR_MESSAGE_STOP,
    CAN_KILO_BOOTPAGE,
    CAN_KILO_BOOTPAGE_SIZE,
    CAN_KILO_BOOTPAGE_NUMBER,
    CAN_MODULE_BOOT,
    CAN_MODULE_BOOTPAGE,
    CAN_MODULE_BOOTPAGE_SIZE,
    CAN_MODULE_BOOTPAGE_NUMBER,
    CAN_MODULE_KILOBOT_SETUP,
    CAN_MODULE_IDLE,
    CAN_MODULE_SETUP,
    CAN_MODULE_RUN,
    CAN_MODULE_PAUSE,                 ///< IGNORED!
    CAN_MODULE_CONFIG_SIZE,           ///< Number of 6 byte configuration data segments
    CAN_MODULE_CONFIG_TRANSFER,       ///< Configuration data transmission
    CAN_TRACKING_KILOBOT,
	CAN_KILOBOT_DATA,
	CAN_MODULE_DATA,
    CAN_TRACKING_KILOBOT_START,
	CAN_TRACKING_REQ
} CAN_message_type_t;

/**** CALLBACKS DEFINITIONS ****/

typedef void (*CAN_message_rx_t)(CAN_message_t *);
typedef void (*CAN_message_tx_success_t)(void);

extern CAN_message_rx_t module_CAN_message_rx;
extern CAN_message_tx_success_t module_CAN_message_tx_success;

/**** FUNCTIONS PROTOTYPES ****/

/**
 * @brief Initialize CAN message structure.
 * 
 * @param m Structure of type CAN_message_t
 */
void init_CAN_message(CAN_message_t *m);

/**
 * @brief Initialize module for CAN transmission.
 * @details Initialize the CAN chip of the module and set the id
 * (x,y) coordinate of the module for future transmission.
 * Setup filters with respect to current device (MODULE/DISPATCHER) [TOCHECK]
 * 
 * @param x_coord x coordinate of the module
 * @param y_coord y coordinate of the module
 */
void init_module_CAN(uint8_t x_coord, uint8_t y_coord);

/**
 * @brief This function modifies the ID field of the CAN message to be sent
 * according to the Kilogrid addressing scheme.
 * 
 * @param message Message to be sent
 * @param dest Address in Kilogrid
 * 
 * @return Returns 1 if succeeded
 */
uint8_t CAN_message_tx(CAN_message_t *message, kilogrid_address_t dest);

/**
 * @brief Function that returns the coordinates associated with the input CAN
 * message
 * 
 * @param m Message to be decoded
 * @return return coordinate of the message via a structure kilogrid_address_t
 */
kilogrid_address_t get_coord_from_CAN_ID(CAN_message_t *m);

/**
 * @brief Serialize CAN tracking message struct into a CAN message.
 * @details The data contained in the former will be compressed according to  * the number of bits required for each field, and assigned to the data field 
 * of the CAN message. Note that the rest of the fields in the data (e.g.
 * address field) will remain untouched.
 * 
 * @param msg Pointer to the CAN tracking message struct
 * @param cell_id Address of the targeted cell [TOCHECK]
 * @param data Pointer to the CAN message that is the target of the
 *                      serialization
 */
void serialize_tracking_message(CAN_message_t* msg, uint8_t cell_id, tracking_user_data_t* data);

#endif
