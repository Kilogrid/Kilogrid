// TODO Credits to kilolib v2

/*
 * serial_packet.h
 *
 * Created: 02-06-15 18:03:23
 *  Author: Anthony
 */

#ifndef SERIAL_PACKET_H_
#define SERIAL_PACKET_H_

#include <stdint.h>
#include "bootpage.h"
#include "serial_speed.h"

/**** DEFINES ****/
#define SERIAL_PACKET_HEADER                            0x55 // // 01010101: packet header signals the beginning of a packet
#define CONFIGURATION_SIZE                              48
#define SERIAL_PACKET_SIZE                              (BOOTPAGE_SIZE+4)

/**** TYPEDEFS AND ENUMS ****/

typedef enum {
	SERIAL_PACKET_STOP = 0x00,
	SERIAL_PACKET_LEDTOGGLE,
	SERIAL_PACKET_KILO_FORWARD_IR_MSG,
	SERIAL_PACKET_KILO_BOOTPAGE,
	SERIAL_PACKET_KILOGRID_COMMAND,
	SERIAL_PACKET_MODULE_FORWARD_CAN_MSG,
	SERIAL_PACKET_MODULE_BOOTPAGE,
	SERIAL_PACKET_MODULE_CONFIGURATION,
	SERIAL_PACKET_TRACKING,
    SERIAL_PACKET_KILO_PROGRAM_SIZE,
    SERIAL_PACKET_MODULE_BOOTLOAD_START,
    SERIAL_PACKET_MODULE_BOOTLOAD_GET_PAGE,
    SERIAL_PACKET_KILO_BOOTLOAD_START,
    SERIAL_PACKET_KILO_BOOTLOAD_GET_PAGE,
    SERIAL_PACKET_DISPATCHER_CONTINUE,
        SERIAL_PACKET_DISPATCHER_AVAILABLE
} serial_packet_type_t;

/*
 *	@brief Structure that defines the format of a serial packet.
 * Packet format: packet types PACKET_STOP, PACKET_LEDTOGGLE, PACKET_FORWARDMSG, PACKET_FORWARDRAWMSG
 *
 * PACKET_HEADER
 * serial_packet_t

 * data:
 * [0]				IR data payload, byte 0
 * [1]				IR data payload, byte 1
 * [2]				IR data payload, byte 2
 * [3]				IR data payload, byte 3
 * [4]				IR data payload, byte 4
 * [5]				IR data payload, byte 5
 * [6]				IR data payload, byte 6
 * [7]				IR data payload, byte 7
 * [8]				IR data payload, byte 8
 * [9]				IR_message_t
 * ...				0
 * [BOOTPAGE_SIZE]  0
 *
 * Checksum
 *
 * ********
 * Packet format: packet type PACKET_BOOTPAGE
 *
 * PACKET_HEADER
 * serial_packet_t: PACKET_BOOTPAGE
 *
 * data:
 * [0]				Page address (number)
 * [1]				Boot page, byte 0
 * ...
 * [BOOTPAGE_SIZE]	Boot page, byte 127 (or BOOTPAGE_SIZE - 1)
 *
 * Checksum
 *
 *	@see IR_message_t, serial_packet_type_t
 */
typedef struct __attribute__((__packed__)) {
	uint8_t header;
	serial_packet_type_t type;
	uint8_t data[BOOTPAGE_SIZE + 1]; // additional byte: bootpage number
	uint8_t checksum;
} serial_packet_raw_t;

typedef struct{
	uint8_t header;
	volatile serial_packet_type_t type;
	uint8_t bootpage_number; // number of bootpage to be sent next
	uint8_t checksum;
} serial_packet_bootpage_req_t;

#endif /* SERIAL_PACKET_H_ */
