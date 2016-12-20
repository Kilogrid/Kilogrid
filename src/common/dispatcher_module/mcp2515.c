// TODO add proper credits

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
// ----------------------------------------------------------------------------


#include <avr/io.h>
#include <util/delay.h>

#include <stdint.h>
#include <avr/pgmspace.h>

#include "mcp2515.h"
#include "mcp2515_defs.h"
#include "IO.h"
#include "utils.h"
#include "debug.h"

#include "platform.h" // used for platform-specific implementation

// -------------------------------------------------------------------------
// read/write on SPI Bus

uint8_t spi_putc( uint8_t data )
{
	// put byte in send-buffer
	SPDR = data;
	
	// wait until byte was send
	while( !( SPSR & (1<<SPIF) ) )
		;
	
	return SPDR;
}

// -------------------------------------------------------------------------
void mcp2515_write_register( uint8_t address, uint8_t data )
{
	SPI_SELECT_CAN();
	
	spi_putc(SPI_WRITE);
	spi_putc(address);
	spi_putc(data);
	
	SPI_RELEASE_CAN();
}

// -------------------------------------------------------------------------
uint8_t mcp2515_read_register(uint8_t address)
{
	uint8_t data;
	
	SPI_SELECT_CAN();
	
	spi_putc(SPI_READ);
	spi_putc(address);
	
	data = spi_putc(0xff);	
	
	SPI_RELEASE_CAN();
	
	return data;
}

// -------------------------------------------------------------------------
void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data)
{
	SPI_SELECT_CAN();
	
	spi_putc(SPI_BIT_MODIFY);

	spi_putc(address);
	spi_putc(mask);
	spi_putc(data);
	
	SPI_RELEASE_CAN();
}

// ----------------------------------------------------------------------------
uint8_t mcp2515_read_status(uint8_t type)
{
	uint8_t data;
	
	SPI_SELECT_CAN();
	
	spi_putc(type);
	data = spi_putc(0xff);
	
	SPI_RELEASE_CAN();
	
	return data;
}

// -------------------------------------------------------------------------
uint8_t init_mcp2515(uint8_t speed)
{
	
	SET_AS_OUTPUT(nCS_CAN);
	
	SET_AS_OUTPUT(SPI_SCK);
	SET_AS_OUTPUT(SPI_MOSI);
	SET_AS_INPUT(SPI_MISO);
	
	SET_AS_INPUT(nCAN_INT);
	
	// active SPI master interface
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
	SPSR = (1<<SPI2X);
	/*
	SPCR = (1<<SPE)|(1<<MSTR) | (0<<SPR1)|(1<<SPR0);
	SPSR = 0;
	*/
	
	// reset MCP2515 by software reset.
	// After this, the MCP2515 is in configuration mode.
	SPI_SELECT_CAN();
	spi_putc(SPI_RESET);
	SPI_RELEASE_CAN();
	
	// wait a little bit until the MCP2515 has restarted
	_delay_us(10);
	
    SPI_SELECT_CAN();
	spi_putc(SPI_RESET);
	SPI_RELEASE_CAN();
	
	/**** BITRATE CONFIGURATION ****/
	
	// configure CNF3..1 registers sequentially
	SPI_SELECT_CAN();
	spi_putc(SPI_WRITE);
	spi_putc(CNF3); // register address 0x28
	
	spi_putc((1<<PHSEG21)); // PHSEG2<2:0> = 010 --> tPS2 = (2 + 1).TQ = 3TQ
	spi_putc((1<<BTLMODE)|(1<<PHSEG11)); // register address 0x29 (CNF2). PHSEG1<2:0> = 010 --> tPS1 = 3TQ, and BTLMODE = 1: PS2 is determined by PHSEG2<2:0>
    spi_putc(speed); // register address 0x2A (CNF1). BRP<5:0> = speed --> TQ = (1 + speed) / FOSC.    SJW<1:0> = 00 --> SJW = 1TQ (default - increase only if the oscillators of the nodes are unstable). PRSEG<2:0> = 0 --> tPRSEG = 1TQ (default)
	
	// Note: tSync is fixed to 1TQ for the mcp2515.
	
	// Nominal bitrate: tbit = tSync + tPRSEG + tPS1 + tPS2, and NBR = 1 / tbit [kbps]

	// activate receive interrupts for both receive buffers
	spi_putc((1<<RX1IE)|(1<<RX0IE));
	SPI_RELEASE_CAN();
	
	// test if we could read back the value => is the chip accessible?
	if (mcp2515_read_register(CNF1) != speed) {
		return false;
	}
	
	// disable the RXnBF Pins (High Impedance State)
	mcp2515_write_register(BFPCTRL, 0);
	
	// set TXnRTS as inputs
	mcp2515_write_register(TXRTSCTRL, 0);
	
	//mcp2515_write_register(RXB0CTRL, (1<<RXM1)|(1<<RXM0)); // turn off filtering for buffer 0 => receive any message
	//mcp2515_write_register(RXB1CTRL, (1<<RXM1)|(1<<RXM0)); // turn off filtering for buffer 1 => receive any message
	
	mcp2515_write_register(RXB0CTRL, (1<<RXM0)); // receive only standard messages in receive buffer 0
	mcp2515_write_register(RXB1CTRL, (1<<RXM0)); // receive only standard messages in receive buffer 1
	
	// reset device to normal mode
	mcp2515_write_register(CANCTRL, 0);
	return true;
}



// ----------------------------------------------------------------------------
// check if there are any new messages waiting
uint8_t mcp2515_check_message(void) {
	return !IS_SET(nCAN_INT);
}

// ----------------------------------------------------------------------------
// check if there is a free buffer to send messages

uint8_t mcp2515_check_free_buffer(void)
{
	uint8_t status = mcp2515_read_status(SPI_READ_STATUS);
	
	if ((status & 0x54) == 0x54) {
		// all buffers used
		return false;
	}
	
	return true;
}

// ----------------------------------------------------------------------------
uint8_t mcp2515_get_message(CAN_message_t *message)
{
	// read status
	uint8_t status = mcp2515_read_status(SPI_RX_STATUS);
	uint8_t addr;
	uint8_t t;

	if ( BIT_IS_SET(status,6) ) {
		// message in buffer 0
		addr = SPI_READ_RX;
	}
	else if ( BIT_IS_SET(status,7) ) {
		// message in buffer 1
		addr = SPI_READ_RX | 0x04;
	}
	else {
		// Error: no message available
		return 0;
	}

	SPI_SELECT_CAN();
	spi_putc(addr);
	
	// read id
	message->id  = (uint16_t) spi_putc(0xff) << 3;
	message->id |=            spi_putc(0xff) >> 5;
	
	spi_putc(0xff);
	spi_putc(0xff);
	
	// read DLC
	uint8_t length = spi_putc(0xff) & 0x0f;
	
	message->header.length = length;
	message->header.rtr = (BIT_IS_SET(status, 3)) ? 1 : 0;
	
	// read data
	for (t = 0; t < length; t++) {
		message->data[t] = spi_putc(0xff);
	}
	
	SPI_RELEASE_CAN(); // interrupt bits should be cleared by releasing the nCS_CAN signal
	
	status = mcp2515_read_status(SPI_RX_STATUS);
	
	// clear interrupt flag
	// at this point, we know that either BIT(status, 6) == 1 or BIT(status, 7) == 1
// 	if (BIT_IS_SET(status, 6)) {
// 		mcp2515_bit_modify(CANINTF, (1<<RX0IF), 0);
// 	}
// 	else {
// 		mcp2515_bit_modify(CANINTF, (1<<RX1IF), 0);
// 	}
	
	return (status & 0x07) + 1;
}

// ----------------------------------------------------------------------------
uint8_t mcp2515_send_message(CAN_message_t *message)
{
	uint8_t status = mcp2515_read_status(SPI_READ_STATUS);
	
	/* Statusbyte:
	 * 
	 * Bit	Function
	 *  2	TXB0CTRL.TXREQ
	 *  4	TXB1CTRL.TXREQ
	 *  6	TXB2CTRL.TXREQ
	 * Note: the TXREQ bit ....
	*/

	uint8_t address;
	uint8_t t;
	if (BIT_IS_CLEAR(status, 2)) {
		address = 0x00;
	}
	else if (BIT_IS_CLEAR(status, 4)) {
		address = 0x02;
	} 
	else if (BIT_IS_CLEAR(status, 6)) {
		address = 0x04;
	}
	else {
		// all buffer used => could not send message
		return 0;
	}
	
	SPI_SELECT_CAN();
	spi_putc(SPI_WRITE_TX | address);
	
	spi_putc(message->id >> 3);
    spi_putc(message->id << 5);
	spi_putc(0);
	spi_putc(0);

	uint8_t length = message->header.length & 0x0f;
	
	if (message->header.rtr) {
		// an rtr-frame has a length, but contains no data
		spi_putc((1<<RTR) | length);
	}
	else {
		// set message length
		spi_putc(length);
		
		// send data
		for (t = 0; t < length; t++) {
			spi_putc(message->data[t]);
		}
	}
	SPI_RELEASE_CAN();
	
	_delay_us(1);
	
	// send message
	SPI_SELECT_CAN();
	address = (address == 0) ? 1 : address;
	spi_putc(SPI_RTS | address);
	SPI_RELEASE_CAN();
	
	return address;
}

void mcp2515_set_mode(CAN_device_mode_t mode){
	mcp2515_write_register(CANCTRL, (mode & 0b111) << 5); // CAN modes: bits REQOP<2:0> = CANCTRL<7:5>
}

void mcp2515_set_mask(uint8_t mask_num, uint16_t mask){
	uint8_t mask_register_address = 0; // RXMnSIDH. RXMnSIDL = mask_register_addres + 1
	
	mask &= 0b11111111111; // bit guard: all masks must be 11 bits long
	
	mcp2515_set_mode(CAN_MODE_CONFIG); // necessary to modify ID masks and filters
	
	switch(mask_num){
		case 0:
			mask_register_address = RXM0SIDH; // mask register 0
			break;
		case 1: 
			mask_register_address = RXM1SIDH; // mask register 1
			break;
		default: 
			return;
	}
	
	mcp2515_write_register(mask_register_address,  	  (mask & 0b11111111000) >> 3);
	mcp2515_write_register(mask_register_address + 1, (mask & 0b00000000111) << 5); // mask register bits RXMnSIDL<7:5> are the LSBs of the mask

	mcp2515_set_mode(CAN_MODE_NORMAL);
}

void mcp2515_set_filter(uint8_t filter_num, uint16_t filter){
	
	uint8_t filter_register_address;
	uint8_t mcp_register_byte;
	filter &= 0b11111111111; // bit guard: all filters must be 11 bits long
	
	mcp2515_set_mode(CAN_MODE_CONFIG); // necessary to modify ID masks and filters
	
	// look-up table for the address of the register associated with current filter
	if(filter_num <= 2){
		filter_register_address = filter_num * 4; // 0x00, 0x04 or 0x08 for RXF0SIDH, RXF1SIDH, RXF2SIDH
	}
	else if(filter_num >= 3 && filter_num <= 5){
		filter_register_address = ((filter_num-3) * 4) + RXF3SIDH; // 0x10, 0x14 or 0x18 for RXF3SIDH, RXF4SIDH, RXF5SIDH
	}
	else{
		return; // error: escape function without touching the registers
	}
	
	mcp_register_byte = (uint8_t)((filter & 0b11111111000) >> 3); // take the MSBs of the filter
	
	mcp2515_write_register(filter_register_address,  mcp_register_byte); // MSB of the filter go in RXFnSIDH
	
	mcp_register_byte = (uint8_t)((filter & 0b00000000111) << 5); // take the 3 LSBs of filter, and shift from 5
	
	mcp2515_write_register(filter_register_address + 1, mcp_register_byte); // filter register bits RXFnSIDL<7:5> are the LSBs of the filter

	mcp2515_set_mode(CAN_MODE_NORMAL);
}