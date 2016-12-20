/** \file IO.h
 *
 * @brief These macros allow the user to set ports directions and bits based on the generic names
 * of the signals on the cell. Generic names are: IRRXCOM_A,D ; IRRXADDR0,1,2 ; IRTXCOM ; IRTXADDR0.1 ; etc...
 *
 * Usage: SET_AS_OUTPUT(IRTXCOM);
 *
 */

#ifndef KILOGRID_MODULE_IO
#define KILOGRID_MODULE_IO

#include <stdlib.h>


/**
 * @brief Write a 1 in place of the specified bit in the direction register.
 */
#define SET_AS_OUTPUT(o)\
	o##dir |= (1 << o##bit);

/**
 * @brief Write a 0 in place of the specified bit in the direction register
 */
#define SET_AS_INPUT(o)\
	o##dir &= ~(1 << o##bit);

/**
 * @brief Write a 1 in place of the specified bit in the port register
 */
#define PIN_HIGH(o)\
	o##port |= (1 << o##bit);

/**
 * @brief Write a 0 in place of the specified bit in the port register
 */
#define PIN_LOW(o)\
	o##port &= ~(1 << o##bit);

/**
 * @brief Check if the specified bit is set.
 */
#define IS_SET(o)\
(o##port & (1 << o##bit)) != 0


/* IR MUX address bit selection */

/**
 * @brief Write IR TX MUX address bits. Connects the IRTXCOM signal to one of the
 * outputs of the MUX based on the "addr" bits.
 */
#define IR_TX_MUX_SELECT(addr) {\
	IRTXADDRport = ( ~(1 << IRTXADDR0bit) & ~(1 << IRTXADDR1bit) )\
	| (addr << IRTXADDR0bit);\
}

/**
 * @brief Write IR RX MUX address bits. Connects the IRRXCOM signal to one of the
 * inputs of the MUX based on the "addr" bits.
 */
#define IR_RX_MUX_SELECT(addr) {\
	IRRXADDRport = ( ~(1 << IRRXADDR0bit) & ~(1 << IRRXADDR1bit) & ~(1 << IRRXADDR2bit) )\
	| (addr << IRRXADDR0bit);\
}

/**
 * @brief Initialize all ports.
 */
#define ports_off() \
	DDRB = 0;   \
	DDRC = 0;	\
	DDRD = 0;	\
	\
	PORTB = 0;	\
	PORTC = 0;	\
	PORTD = 0;	\


/*
 * Definition of all IOs used in the cell.
 * The definitions use generic names, and append the suffixes "port", "bit" and "dir"
 * to the generic names to designate the related registers in the microcontroller.
 * Note that these definitions depend on the microcontroller used (Cell Revision A: ATmega328P).
 */

/* MCU */
#define nRESETbit		PORTC6
#define nRESETport		PORTC
#define nRESETdir		DDRC

/* IR */

// IO pins
#define IRTXCOMbit		PORTB1
#define IRTXADDR0bit	PORTD3
#define IRTXADDR1bit	PORTD4

#define IRRXCOM_Dbit	PORTC0 // digital input
#define IRRXCOM_Abit	PORTD6 // analog input
#define IRRXADDR0bit	PORTC1
#define IRRXADDR1bit	PORTC2
#define IRRXADDR2bit	PORTC3

// ports
#define IRTXCOMport 	PORTB
#define IRTXADDRport	PORTD // allows to act on all TX ADDR bits
#define IRTXADDR0port	IRTXADDRport // for initialization only
#define IRTXADDR1port	IRTXADDRport // for initialization only

#define IRRXCOM_Dport 	PORTC
#define IRRXCOM_Aport 	PORTD
#define IRRXADDRport	PORTC
#define IRRXADDR0port	PORTC // for initialization only
#define IRRXADDR1port	PORTC // for initialization only
#define IRRXADDR2port	PORTC // for initialization only

// direction registers
#define IRTXCOMdir 		DDRB
#define IRTXADDR0dir	DDRD // for initialization only
#define IRTXADDR1dir	DDRD // for initialization only

#define IRRXCOM_Ddir 	DDRC
#define IRRXCOM_Adir 	DDRD
#define IRRXADDR0dir	DDRC // for initialization only
#define IRRXADDR1dir	DDRC // for initialization only
#define IRRXADDR2dir	DDRC // for initialization only

/* CAN */

// IO pins
#define CAN_CLKbit		PORTB0
#define nCS_CANbit		PORTB2
#define nCAN_INTbit		PORTD2

// ports
#define CAN_CLKport		PORTB
#define nCS_CANport		PORTB
#define nCAN_INTport	PORTD

// direction registers
#define CAN_CLKdir		DDRB
#define nCS_CANdir		DDRB
#define nCAN_INTdir		DDRD

/* I2C */

// IO pins
#define I2C_SDAbit		PORTC4
#define I2C_SCLbit		PORTC5

// ports
#define I2C_SDAport		PORTC
#define I2C_SCLport		PORTC

// direction registers
#define I2C_SDAdir		DDRC
#define I2C_SCLdir		DDRC

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

/* Test pins */
// IO pins
#define TEST3bit		SPI_MOSIbit

// ports
#define TEST3port		SPI_MOSIport

// dir
#define TEST3dir		SPI_MOSIdir

/* voltage reference */
#define VOLTAGE_REFbit		PORTD7
#define VOLTAGE_REFport		PORTD
#define VOLTAGE_REFdir		DDRD

#endif  // KILOGRID_MODULE_IO
