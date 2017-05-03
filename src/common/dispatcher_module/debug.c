/**
 * @file     Kilogrid/src/common/dispatcher_module/debug.c
 * @brief    This file provides print functions for debug via serial.
 *   
 * @author   IRIDIA lab
 * @date     May, 2017
 */

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>  


void cprints(char *c)
{
	uint16_t a = 0;
	
	// send new line char only if input char array is not empty
	if(c[a] != 0) USART_SEND(0x0A); //0x0A: New line '\n'
	
	while( (c[a] != 0) && (a < 100) ){
		USART_SEND(c[a]);
		a++;
	}
	
	USART_SEND(0x0A);
	//USART_SEND(0x0D); // 0x0D: Carriage return '\r'
}

void cprinti(int i)//print int for debugging out serial
{
	char buffer[10];
	uint8_t a; 
	
	for(a = 0; a < 10; a++)
		buffer[a] = 0x20; // ASCII 0x20: SPACE

	itoa(i, buffer, 10); // convert integer into an array of char. 
	// buffer[] now contains the ASCII representation of a number,
	// and finishes by a zero character after the last digit.
	
	for(a = 0; a < 10; a++)
	{
			if(buffer[a] == 0) { // zero character detected
				buffer[a] = 0x20; // overwrite the zero character by a SPACE
			}
			USART_SEND(buffer[a]);
			
	}

	USART_SEND(0x20); // ASCII 0x20: SPACE
}