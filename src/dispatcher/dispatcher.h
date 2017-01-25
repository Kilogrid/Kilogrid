/**
 * @file     Kilogrid/src/dispatcher/dispatcher.h
 * @mainpage Kilogrid Library API
 * @brief    Library for the dispatcher of the Kilogrid.
 * @author   IRIDIA lab
 * @date     
 * @copyright licensed under creative commons attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0) 
 *           more info at http://creativecommons.org/licenses/by-nc-sa/3.0/
 */

#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__

#include <stdint.h>


#define MS_PER_TICK             32
#define MS_TO_TICKS(a)          (a / MS_PER_TICK)
#define BAUD_TOL                3 /** avoid warning from avr/setbaud.h since we can't get a 2% tolerance at baudrate 256000 with the ATmega328P at 8 MHz */

/**
 * @brief      test
 *
 * @return     { description_of_the_return_value }
 */
#define disp_timer_setup() {\
    TCCR0A = 0;             /** Normal operation. */\
    TCCR0B = 0;             /** Set prescaler to 0 (disabled). */\
    OCR0A  = 0xFF;          /** Set compare register to 255 (~32.768 ms). */\
    TIMSK0 |= (1<<OCIE0A)   /** Interrupt enable on match output compare register A. */\
    disp_timer_off();\
}

/**
 * @brief      { function_description }
 *
 * @return     { description_of_the_return_value }
 */
#define disp_timer_on() {\
    TCNT0  = 0;             /** Reset counter. */\
    TCCR0B |= (1<<CS02)|(1<<CS00);  /** Set prescaler to 1024 (~ 128us). */\
}

/**
 * @brief      { function_description }
 *
 * @return     { description_of_the_return_value }
 */
#define disp_timer_off() {\
    TCCR0B = 0;             /** Set prescaler to 0 (disabled). */\
    TCNT0  = 0;             /** Reset counter. */\
}

/**
 * @brief      { function_description }
 *
 * @param      c     { parameter_description }
 *
 * @return     { description_of_the_return_value }
 */
#define disp_timer_set_compare(c) {\
    OCR0A  = c;             /** Set compare register to c. */\
    TCNT0  = 0;             /** Reset counter. */\
}

/* TYPEDEFS AND ENUMS */

extern volatile uint32_t disp_ticks;

/* PROTOTYPES */

/**
 * @brief      { function_description }
 */
void dispatcher_init(void);

#endif//__DISPATCHER_H__
