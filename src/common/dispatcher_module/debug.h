/**
 * @file     Kilogrid/src/common/dispatcher_module/debug.h
 * @brief    This file provides print functions for debug via serial.
 * @details  credits to kilobot library 
 * @author   IRIDIA lab
 * @date     May, 2017
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <avr/io.h>        // for port and register definitions

#include "debug.h"
#include "IO.h"
 
// set of macros and functions used for debugging

#define USART_SEND(data) {\
	while(!(UCSR0A & (1<<UDRE0)));/* Wait for USART to be available */\
	UDR0 = data; /* Write data in buffer*/\
}

void init_serial(void);

/**
 * @brief Print data to the serial port (USART).
 * @details The length of the string given to the function is not limited (has 
 * to end by a 0 char).
 * c is the pointer to the string to send.
 * This function takes approximately 39.3 us to send a character with a 
 * baudrate of 256000 symbols per sec on the serial.
 * 
 * @param c char string to print
 */
void cprints(char *c);
void cprints_no_carriage(char *);

/**
 * @brief Print integer data to the serial port (USART).
 * 
 * @param i integer to print
 */
void cprinti(int i);

#endif//__DEBUG_H__
