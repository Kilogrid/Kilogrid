// TODO credits

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
	CAN_TRACKING_REQ,
    CAN_MODULE_TO_MODULE            // TODO, by till, this msg type is used by my messages - to make sure that my msgs have a unique type - if it works probably rename to CAN_MODULE_TO_MODULE
} CAN_message_type_t;

/**** CALLBACKS DEFINITIONS ****/

typedef void (*CAN_message_rx_t)(CAN_message_t *);
typedef void (*CAN_message_tx_success_t)(void);

extern CAN_message_rx_t module_CAN_message_rx;
extern CAN_message_tx_success_t module_CAN_message_tx_success;

/**** FUNCTIONS PROTOTYPES ****/

void init_CAN_message(CAN_message_t *);
void init_ModuleCAN(uint8_t, uint8_t);
uint8_t CAN_message_tx(CAN_message_t *, kilogrid_address_t);
kilogrid_address_t get_coord_from_CAN_ID(CAN_message_t*); // function that returns the coordinates associated with the input CAN message
void serialize_tracking_message(CAN_message_t* msg, uint8_t cell_id, tracking_user_data_t* data);

#endif
