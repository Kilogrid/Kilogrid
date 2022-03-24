/** \file module.h
 *
 * @brief This file implements high level handling of CAN messages based on
 * their type. Commands or bootpages forwarded to the kilobots are
 * processed here. The two functions defined handle the module initialization
 * and the starting of the module as well.
 *
 * Initializing the module means initializing all the peripherals:
 *  - serial
 *  - led driver
 *  - CAN
 *  - IR
 *
 * Starting the module:
 *  - waiting for kilobot bootpages
 *  - transferring the bootpages to the kilobots once complete
 *  - calling user supplied setup, triggered by CAN_MODULE_SETUP message
 *  - calling user supplied loop, triggered by CAN_MODULE_RUN message
 */
#ifndef KILOGRID_MODULE_H
#define KILOGRID_MODULE_H

#include <mcp2515.h>


// module rev.A contains 4 cells
#define N_CELLS                             4

// cells are numbered from CELL_LOWER_INDEX to CELL_HIGHER_INDEX
#define CELL_LOWER_INDEX                    0x00
#define CELL_UPPER_INDEX                    CELL_LOWER_INDEX + (N_CELLS - 1)

/** Size of the buffer that holds configuration. */
#define MODULE_CONFIGURATION_BUFFER_SIZE    48

#define CAN_MESSAGE_BUFFER_SIZE				10
#define MS_TO_TICKS(a)						(a / 32)

typedef void (*AddressPointer_t)(void) __attribute__ ((noreturn));

/**
 *	@brief Enumerates all cells addresses of the module.
 *  The addresses relate to their position in the MUXes and LED driver positions.
 */
typedef enum {
	CELL_00 = CELL_LOWER_INDEX,
	CELL_01 = CELL_LOWER_INDEX + 1,
	CELL_02 = CELL_LOWER_INDEX + 2,
	CELL_03 = CELL_LOWER_INDEX + 3
} cell_num_t;


/**
 * Unique id of the module, used by the entire module lib, and made accessible
 * to the user.
 */
extern volatile uint16_t module_uid;
#ifndef NDEBUG
    extern volatile uint8_t  module_uid_x_coord;
    extern volatile uint8_t  module_uid_y_coord;
#endif  // NDEBUG

/**
 * @brief Can be used for timing purposes, increased every 1/32th of a second.
 * moduleIR.h has TICKS_PER_SEC and SEC_TO_TICKS(a) macros to help handling
 * this.
 *
 * @note Updated in moduleIR.c: ISR(TIMER0_COMPA_vect). Used for IR tx timing.
 */
extern volatile uint32_t module_ticks;

/** Size of configuration data in bytes. */
extern volatile uint8_t  configuration_size;

/** Configuration data. */
extern volatile uint8_t  configuration[MODULE_CONFIGURATION_BUFFER_SIZE];

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/**
 * @brief Initialize module hardware.
 *
 * This function initializes all hardware of the module. This includes setting hardware timers,
 * configuring ports, setting up analog-to-digital converters,
 * registering system interrupts and the initializing the messaging
 * subsystem.
 *
 * It is recommended that you call this function as early as possible
 * inside the `main` function of your program.
 */
void module_init(void);

/**
 * @brief Start module event loop.
 *
 * This function receives two parameters. The first parameter @p setup
 * is a function which will be called once to perform any initialization
 * required by your user program. The second parameter @p loop is a
 * function that will be called repeatedly to perform any computations
 * required by your user program.
 *
 * Using the overhead controller it is possible to interrupt the event
 * loop to trigger events such as program start/resume, program pause,
 * and program restart.
 *
 * @param setup Put your setup code here, it will be called in a loop
 					the configuration for the module (if any) is in the
 					configuration variable, the size of the configuration
 					data is configuration_size bytes. If configuration_size
 					is 0 it means that no configuration data was received.

 * @param loop  Put your main code here, will be run repeatedly.
 *
 * @code
 *
 * uint32_t counter;
 *
 * void setup() {
 *    counter = 0;
 * }
 *
 * void loop() {
 *    counter++;
 * }
 *
 * int main() {
 *   module_init();
 *   module_start(setup, loop);
 *   return 0;
 * }
 * @endcode
 */
void module_start(void (*setup)(void), void (*loop)(void));
void module_enable_autostart();
void CAN_send_broadcast_message(CAN_message_t*);
uint8_t debug_till();
uint16_t get_random(uint16_t low, uint16_t high);

CAN_message_t* next_CAN_message();

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // KILOGRID_MODULE_H
