/** \file message.h */

#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <stdint.h>

/**
 * @brief Possible types of an IR message.
 *
 * Associated LED colors:
 *  - RESET         : GREEN
 *  - BOOT          : BLUE
 *  - BOOTPGM_PAGE  : CYAN
 *  - SLEEP         : WHITE
 *  - RUN           : MAGENTA
 *  - VOLTAGE       : YELLOW
 *  - BOOTPGM_SIZE  : YELLOW
 *  - CHARGE        : RED
 *  - other         : off
 *
 * The enumeration ranges are the following:
 * 0x00 to 0x3F : User-defined IR messages.
 * 0x40 to 0x7F : Module IR messages.
 * 0x80 to 0xFF : Kilobot kilolib v2 configuration messages.
 * Any new type added to this list must respect these conventions,
 * and should be added AFTER the last element of the defined range,
 * to keep the header file compatible with the initial kilolib v2.
 */
typedef enum {
	/**** Kilobot to Kilobot range ****/
    NORMAL = 0,
    GPS,
	TEST = 0x0A, // reserved for tests
	/**** Module to Kilobot range ****/
    TRACKING = 0x10,

    GO_STRAIGHT = 0x7D,
    TURN_LEFT,
    TURN_RIGHT,

	/**** Kilobot configuration range ****/
    BOOT = 0x80,
    BOOTPGM_PAGE,
    BOOTPGM_SIZE,
    RESET,
    SLEEP,
    WAKEUP,
    CHARGE,
    VOLTAGE,
    RUN,
    READUID,
    CALIB
} IR_message_type_t;

enum {
    CALIB_SAVE,
    CALIB_UID,
    CALIB_TURN_LEFT,
    CALIB_TURN_RIGHT,
    CALIB_STRAIGHT
};

#ifdef KILOGUI
#pragma pack(1)
#endif

typedef  struct  __attribute__((__packed__)) {
    uint8_t mode;
    uint16_t uid;
    uint8_t turn_left;
    uint8_t turn_right;
    uint8_t straight_left;
    uint8_t straight_right;
    uint16_t unused;
} calibmsg_t;

/**
 * @brief Message structure.
 *
 * A message structure is 12 bytes in length and is composed of three
 * parts: the payload (9 bytes), the message type (1 byte), and a CRC (2
 * bytes).
 *
 * @note When preparing a message for transmission, at a minimum you
 * must specify the type (use a value between 0 and 127 for user
 * messages) and the CRC (use the message_crc() function for this
 * purpose).
 *
 * @see message_crc, kilo_message_rx, module_message_tx_success
 */
typedef struct {
    uint8_t data[9]; ///< message payload
    uint8_t type;    ///< message type
    uint16_t crc;    ///< message crc
} IR_message_t;

#endif//__MESSAGES_H__
