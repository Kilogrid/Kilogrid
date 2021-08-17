/** \file debug.h */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <avr/io.h>        // for port and register definitions

// TODO credits for the functions: libkilobot v1 !!!!

// set of macros and functions used for debugging

#define USART_SEND(data) {\
	while(!(UCSR0A & (1<<UDRE0)));/* Wait for USART to be available */\
	UDR0 = data; /* Write data in buffer*/\
}

void init_serial(void);
void cprints(char *);
void cprints_no_carriage(char *);
void cprinti(int);

#endif//__DEBUG_H__
