/* Copyright (c) 2007 Fabian Greif
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/** \file mcp2515.h */

#ifndef	MCP2515_H
#define	MCP2515_H

#include "stdint.h"
#include "bootpage.h"
#include "mcp2515_defs.h"
#include "kilogrid.h"

/**** DEFINES ****/
#define SPI_SELECT_CAN()			PIN_LOW(nCS_CAN)
#define SPI_RELEASE_CAN()			PIN_HIGH(nCS_CAN)

/**
BRP<5:0> bits of the CNF1 configuration register
The Bitrate prescaler bits determine the nominal time quantum of the chip, and, therefore, the bit time.
--> TQ = (1 + CANSPEED) / FOSC
Note: FOSC = 8 MHz for the module and dispatcher.
See the init_mcp2515 function for more detail on the configuration of the bitrate of the CAN controller.
*/
#define CANSPEED_125	3
#define CANSPEED_250	1
#define CANSPEED_500	0

/** Size of the data section in a CAN_message_t in bytes */
#define CAN_MESSAGE_SIZE							8
#define CAN_MESSAGE_PAYLOAD							(CAN_MESSAGE_SIZE - 2) ///< First two bytes are the CAN message type and packet number
#define CAN_MESSAGES_PER_BOOTPAGE					(((BOOTPAGE_SIZE+1) / (CAN_MESSAGE_PAYLOAD)) + 1) // 22

/*** TYPEDEFS AND ENUMS ****/

/** Structure of a CAN message */
typedef struct{
	uint16_t id;

	struct {
		int8_t rtr : 1;
		uint8_t length : 4;
	} header;

	uint8_t data[CAN_MESSAGE_SIZE];

} CAN_message_t;

/**** PROTOTYPES ****/

uint8_t spi_putc(uint8_t);
uint8_t init_mcp2515(uint8_t);
uint8_t mcp2515_read_register(uint8_t);
uint8_t mcp2515_read_status(uint8_t);
uint8_t mcp2515_check_message(void); ///< check if there are any new messages waiting
uint8_t mcp2515_check_free_buffer(void); ///< check if there is a free buffer to send messages
uint8_t mcp2515_get_message(CAN_message_t *);
uint8_t mcp2515_send_message(CAN_message_t *);

// NOTE: these functions assume standard, 11-bit identification scheme
void mcp2515_write_register(uint8_t, uint8_t);
void mcp2515_bit_modify(uint8_t, uint8_t, uint8_t);
void mcp2515_set_mask(uint8_t, uint16_t);
void mcp2515_set_filter(uint8_t, uint16_t);
void mcp2515_set_mode(CAN_device_mode_t);

#endif	// MCP2515_H
