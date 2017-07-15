/**
 * @file     Kilogrid/src/dispatcher/dispatcher.h
 * @brief    Implements the main state machine function of the dispatcher device.
 * @details  This program has to be run on the dispatcher device. It implements
 * the main state machine that will run endlessly. 
 * We recall that the dispatcher aims at interfacing the KiloGUI application 
 * that communicates via USB with the Kilogrid arena's modules communicating 
 * via CAN messages. We also recall that the dispatcher can replace the OHC 
 * and all its functionality.
 * 
 * This program implements two types of communication: forwarding and polling.
 * If no experiment is running, messages from the KiloGUI (USB) are forwarded 
 * to desire device on CAN network. When an experiment is running, the 
 * dispatcher coordinates communication, via polling mechanism to prevent 
 * collisions.
 * Type of messages managed by the program are:
 *      - SERIAL_PACKET_DISPATCHER_CONTINUE: continue serial transmission
 *      - SERIAL_PACKET_STOP: stops the serial and IR communication
 *      - SERIAL_PACKET_KILO_FORWARD_IR_MSG: prepare messages to be sent to 
 *      Kilobot via the infrared of the modules of the Kilogrid arena
 *      - SERIAL_PACKET_KILO_BOOTLOAD_START: start boot program of Kilobots
 *      - SERIAL_PACKET_KILO_BOOTPAGE: sends bootpage program to Kilobots
 *      - SERIAL_PACKET_MODULE_CONFIGURATION: configure module adresses and 
 *      type
 *      - SERIAL_PACKET_KILOGRID_COMMAND: sends specific command to the arena:
 *          - KILOGRID_STOP_IR: stop infrared communication
 *          - KILOGRID_IDLE: set the kilogrid in idle mode 
 *          - KILOGRID_SETUP: trigger the call for user setup
 *          - KILOGRID_RUN_EXP: start the experiment (triger the user loop)
 *          - KILOGRID_BOOTLOAD: start boot program of the Kilogrid
 *      - SERIAL_PACKET_MODULE_BOOTLOAD_START: start boot program of a module
 *      - SERIAL_PACKET_MODULE_BOOTPAGE: sends program to a module
 * 
 * @author   IRIDIA lab
 * @date     January 2017
 */

#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__

#include <bootldr.h>
#include <kilogrid.h>
#include <debug.h>
#include <CAN.h>
#include <serial_packet.h>
#include <tracking.h>
#include <ringbuffer.h>

#include "IO.h"

#include <stdint.h>
#include <string.h>        // for memcpy
#include <avr/io.h>        // for port and register definitions
#include <avr/interrupt.h> // for ISR
#include <util/setbaud.h>
#include <util/delay.h>    // for _delay_ms

#define MS_PER_TICK             32
#define MS_TO_TICKS(a)          (a / MS_PER_TICK)
#define BAUD_TOL                3 /** avoid warning from avr/setbaud.h since we can't get a 2% tolerance at baudrate 256000 with the ATmega328P at 8 MHz */

/** Setup the timer of the dispatcher */
#define disp_timer_setup() {\
    TCCR0A = 0;             /** Normal operation. */\
    TCCR0B = 0;             /** Set prescaler to 0 (disabled). */\
    OCR0A  = 0xFF;          /** Set compare register to 255 (~32.768 ms). */\
    TIMSK0 |= (1<<OCIE0A);   /** Interrupt enable on match output compare register A. */\
    disp_timer_off();\
}

/** Start the timer of the dispatcher */
#define disp_timer_on() {\
    TCNT0  = 0;             /** Reset counter. */\
    TCCR0B |= (1<<CS02)|(1<<CS00);  /** Set prescaler to 1024 (~ 128us). */\
}

/** Stop the timer of the dispatcher */
#define disp_timer_off() {\
    TCCR0B = 0;             /** Set prescaler to 0 (disabled). */\
    TCNT0  = 0;             /** Reset counter. */\
}

/** Comparison on the timer of the dispatcher */
#define disp_timer_set_compare(c) {\
    OCR0A  = c;             /** Set compare register to c. */\
    TCNT0  = 0;             /** Reset counter. */\
}

/* TYPEDEFS AND ENUMS */

extern volatile uint32_t disp_ticks;

/* PROTOTYPES */

/**
 * @brief      Function to initialize the dispatcher of the Kilogrid.
 */
void dispatcher_init(void);

/**
 * @brief      Empty dummy callback for the CAN tx success handling.
 */
void CAN_message_tx_success_dummy();

/**
 * @brief      Initialize uart for output of serial debugging info.
 */
void init_serial(void);

/**
 * @brief      Receive a message to modules via CAN.
 */
void CAN_rx(CAN_message_t *m);

/**
 * @brief      Set succes of CAN transmission.
 */
void CAN_tx_success(uint8_t success);

/**
 * @brief      Send bootpage toward modules via CAN messages.
 */
void CAN_send_bootpage(bootpage_data_bytes_t *bootpage, uint8_t bootpage_number);

/**
 * @brief      Main (infinite) loop of the module.
 */
int main();

#endif//__DISPATCHER_H__
