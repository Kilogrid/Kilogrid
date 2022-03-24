/*
 * IO.h
 *
 * Created: 5/19/2015 17:50:02
 *  Author: Anthony Antoun
 */ 

#ifndef IO_H_
#define IO_H_

#include <stdlib.h>  

/**** MACROS ****/

/*
 *	These macros allow the user to set ports directions and bits based on the generic names
 * of the signals on the cell. Generic names are: IRRXCOM_A,D ; IRRXADDR0,1,2 ; IRTXCOM ; IRTXADDR0.1 ; etc...
 * 
 * Usage: SET_AS_OUTPUT(DEBUG_1);
 * 
 */

/*
 *	@brief Write a 1 in place of the specified bit in the direction register.
 */
#define SET_AS_OUTPUT(o)\
	o##dir |= (1 << o##bit); 

/*
 *	@brief Write a 0 in place of the specified bit in the direction register
 */
#define SET_AS_INPUT(o)\
	o##dir &= ~(1 << o##bit); 

/*
 *	@brief Write a 1 in place of the specified bit in the port register
 */
#define PIN_HIGH(o)\
	o##port |= (1 << o##bit);

/*
 *	@brief Write a 0 in place of the specified bit in the port register
 */
#define PIN_LOW(o)\
	o##port &= ~(1 << o##bit); 

/*
 *	@brief Check if the specified bit is set.
 */
#define IS_SET(o)\
(o##port & (1 << o##bit)) != 0

/**** DEFINES ****/

/*
 * Definition of all IOs used in the cell. 
 * The definitions use generic names, and append the suffixes "port", "bit" and "dir" 
 * to the generic names to designate the related registers in the microcontroller. 
 * Note that these definitions depend on the microcontroller used (Cell Revision A: ATmega328P).
 */

/* DEBUG */

// IO pins
#define DEBUG_0bit		PORTB0
#define DEBUG_1bit		PORTB1	
#define DEBUG_2bit		PORTB2	

// ports
#define DEBUG_0port		PORTB
#define DEBUG_1port		PORTB
#define DEBUG_2port		PORTB

// direction registers
#define DEBUG_0dir		DDRB
#define DEBUG_1dir		DDRB
#define DEBUG_2dir		DDRB

/* CAN */

// IO pins
#define nCS_CANbit		PORTD7
#define nCAN_INTbit		PORTD2
#define nCAN_EN1bit		PORTD3
#define nCAN_EN2bit		PORTD4

// ports
#define nCS_CANport		PORTD
#define nCAN_INTport	PORTD
#define nCAN_EN1port	PORTD
#define nCAN_EN2port	PORTD

// direction registers
#define nCS_CANdir		DDRD
#define nCAN_INTdir		DDRD
#define nCAN_EN1dir		DDRD
#define nCAN_EN2dir		DDRD

/* SPI */

// IO pins
#define	SPI_MOSIbit		PORTB3
#define SPI_MISObit		PORTB4
#define SPI_SCKbit 		PORTB5

// ports
#define	SPI_MOSIport	PORTB
#define SPI_MISOport	PORTB
#define SPI_SCKport 	PORTB

// direction registers
#define	SPI_MOSIdir		DDRB
#define SPI_MISOdir		DDRB
#define SPI_SCKdir 		DDRB

/* Serial */

// IO pins
#define RXDbit			PORTD0
#define TXDbit			PORTD1

// ports
#define RXDport			PORTD
#define TXDport			PORTD

// dir
#define RXDdir			DDRD
#define TXDdir			DDRD

/* PROGRAMMER (AVRISP) */
// IO pins
#define nPROGRAMMER_RESETbit		PORTC1

// ports
#define nPROGRAMMER_RESETport		PORTC

// dir
#define nPROGRAMMER_RESETdir		DDRC

/* FTDI */

// IO pins
#define nRESET_FTDIbit		PORTC2

// ports
#define nRESET_FTDIport		PORTC

// ports
#define nRESET_FTDIdir		DDRC

/* HUB */
// IO pins
#define nHUB_RESETbit		PORTC5

// ports
#define nHUB_RESETport		PORTC

// dir
#define nHUB_RESETdir		DDRC


#endif /* IO_H_ */