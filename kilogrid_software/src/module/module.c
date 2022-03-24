#include <stdlib.h>         // for rand()
#include <time.h>
#include <string.h>         // for memcpy
#include <avr/wdt.h>        // watch dog timer
#include <avr/interrupt.h>
#include <avr/eeprom.h>     // read eeprom values
#include <util/delay.h>     // delay macros

#include "module.h"
#include "moduleIR.h"
#include "moduleLED.h"
#include "ADC.h"
#include "IO.h"
#include "eeprom.h"

#include <kilogrid.h>
#include <bootldr.h>
#include <debug.h>
#include <rand.h>
#include <CAN.h>
#include <message.h>
#include <message_crc.h>
#include <ringbuffer.h>


/* External variables */
volatile uint16_t module_uid;       ///< module identifier
volatile uint32_t module_ticks;     ///< internal clock (updated in tx ISR)


/* Source local variables */
/**
 * @brief Module states
 */
static volatile enum {
	MODULE_INIT = 0x00,
	/**
	 * @brief Triggered by receiving a CAN_MODULE_IDLE message. Currently
	 * pulsing LEDs indicate this state.
	 */
	MODULE_IDLE,
	/**
	 * @brief State after receiving the second part of a CAN_FORWARD_IR_MESSAGE
	 * message, transmits the message in IR_setup_message variable on all
	 * modules.
	 */
	MODULE_SETUP_KILOBOTS,
	/**
	 * @brief Module is waiting for bootpages.
	 *
	 * Triggered by receiving a CAN_KILO_BOOTPAGE_NUMBER CAN message.
	 */
	MODULE_WAITING_FOR_KILOBOT_BOOTPAGE,
	/**
	 * @brief Sends the current bootpage in 6 byte chunks to the kilobots.
	 *
	 * Triggered after receiving the last bootpage on the CAN bus.
	 */
	MODULE_BOOTLOADING_KILOBOTS,
	/**
	 * @brief Received information about the configuration and is waiting for the data.
	 *
	 * Triggered by receiving a CAN_MODULE_CONFIG_SIZE message.
	 */
	MODULE_WAITING_FOR_CONFIGURATION_DATA,
	/**
	 * @brief State entered when the module is about to start and sets up all
	 * user variables before start (via callind the user supplied setup
	 * funcion).
	 *
	 * Triggered by receiving a CAN_MODULE_SETUP message.
	 */
	MODULE_SETUP,
	/**
	 * @brief Calls the user supplied loop function.
	 *
	 * Triggered by receiving a CAN_MODULE_RUN message.
	 */
    MODULE_RUNNING
} module_state;  // internal state of module

#ifdef NDEBUG
#   define _static static
#else
#   define _static
#endif
/*
 * coordinate of the module in the grid (determined at programming time,
 * stored in EEPROM)
 */
_static volatile uint8_t module_uid_x_coord;      // => LSBs of module_uid
_static volatile uint8_t module_uid_y_coord;      // => MSBs of module_uid

/* cariblation area */
static volatile uint8_t module_calib_restricted;  // flag for Caliblation area
static volatile uint8_t module_calib_x_min;       // x lower bound
static volatile uint8_t module_calib_x_max;       // x upper bound
static volatile uint8_t module_calib_y_min;       // y lower bound
static volatile uint8_t module_calib_y_max;       // y upper bound

/** @brief To jump directly to the user program without the user intervention */
static volatile uint8_t module_autostart = 0;

static volatile color_t c;						// color of the module during configuration

CAN_message_t CAN_message_rx;			// store received message
CAN_message_t CAN_message_bootpage;		// CAN message that stores bootpage messages

/** @brief CAN messages buffer that contains the CAN user messages to be sent. */
RB_create(CAN_message_tx_buffer, CAN_message_t, CAN_MESSAGE_BUFFER_SIZE);

/**
 * @brief Stores received message until process_CAN_message transfers it into
 * either CAN_message_rx or CAN_message_bootpage depending on the type if the
 * message.
 */
CAN_message_t CAN_message_rx_buffer;	// store received message from interrupt

/**
 * @brief Type of the received CAN message. The switch in process_CAN_message is
 * done based on this value.
 */
volatile CAN_message_type_t CAN_msg_type;
volatile uint8_t fCAN_debug_rx;

/**
 * @brief Stores the number of bootpages contained in the bootloader.
 * Triggered by a CAN_FORWARD_IR_MESSAGE IR message with IR message type
 * BOOTPGM_SIZE.
 */
volatile uint8_t page_total;

/**
 * @brief Index of current bootpage we are receiving
 */
volatile uint8_t page_count = 0;

/**
 * @brief Address of current bootpage, sent in a CAN_KILO_BOOTPAGE_NUMBER CAN message.
 */
volatile uint8_t page_address;

/**
 * @brief Position of the next data byte in the current_bootpage.data array.
 */
volatile uint8_t page_offset;

/**
 * @brief Indicates if current bootpage was sent (send only once).
 */
volatile uint8_t bootpage_sent;

/**
 * @brief Indicates whether a received bootpage was faulty, should not be sent
 * to the kilobot.
 */
volatile uint8_t bootpage_faulty;

/**
 * @brief Used to send the Kilobot program with the different cells, and switch from one cell to the other after sending the whole program
 */
volatile cell_num_t bootloading_current_cell = CELL_00;

/**
 * @brief Bootpage data is loaded into this buffer from the CAN messages.
 */
volatile bootpage_data_bytes_t current_bootpage;

volatile uint8_t previous_CAN_msg_number;

/**
 * @brief Struct that carries the message to send to the kilobots during the
 * setup phase, used by CAN_FORWARD_IR_MESSAGE of process_CAN_message.
 */
IR_message_t IR_setup_message;

volatile uint8_t received_setup;

/** @brief Brightness of the LEDs in idle state. */
volatile uint8_t brightness_idle = 0;

/** @brief Indicates if the brightness is decreasing (-1) or increasing (1). */
volatile int8_t brightness_dir;

/** @brief 1 if not all configuration data was received. */
volatile uint8_t configuration_faulty;

/** @brief Value from 1 to 8: number of CAN messages with configuration data. */
volatile uint8_t configuration_total;

/** @brief ID of the currently received configuration segment. */
volatile uint8_t configuration_msg_counter;

/** @brief Size of configuration data in bytes. */
volatile uint8_t configuration_size;

/** @brief Configuration data. */
volatile uint8_t configuration[MODULE_CONFIGURATION_BUFFER_SIZE];

/** @brief To execute code once before running the loop() variable */
volatile uint8_t has_started;

volatile uint8_t messages_to_send = 0;

volatile uint8_t sending_tracking_data = 0;

volatile uint8_t sent_tracking_header = 0;

kilogrid_address_t CAN_address_to_dispatcher;
kilogrid_address_t CAN_address_to_modules;

volatile uint8_t poll_debug_led_toggle = 0;

CAN_message_t poll_response_message;

/**** FUNCTION POINTERS DECLARATIONS ****/

/** @brief Empty dummy callback function for CAN rx handling */
void CAN_message_rx_dummy(CAN_message_t *m) { }

/** @brief Empty dummy callback function for IR rx handling */
void IR_message_rx_dummy(IR_message_t *m, cell_num_t c, distance_measurement_t *d, uint8_t CRC_error) { ; }

/** @brief Empty dummy callback function for IR tx handling */
void IR_message_tx_success_dummy(cell_num_t c) {;}

/**
 *  @brief Empty dummy callback for the CAN tx success handling.
 */
void CAN_message_tx_success_dummy(){}

/**
 * @brief CAN message received callback.
 * Registering an empty dummy is necessary before receiving CAN messages.
 * Otherwise CAN messages would make the program fail when initializing.
 *
 * @note It is forbidden to use blocking calls in this function, as well as the moduleLED.c functionalities (too slow).
 */
CAN_message_rx_t module_CAN_message_rx = CAN_message_rx_dummy;

/**
 * @brief CAN message tx success callback.
 *
 * @note It is forbidden to use blocking calls in this function, as well as the moduleLED.c functionalities (too slow).
 */
CAN_message_tx_success_t module_CAN_message_tx_success = CAN_message_tx_success_dummy;

/**
 * @brief IR message received callback.
 * Registering an empty dummy is necessary before receiving IR messages.
 * Otherwise IR messages would make the program fail when initializing.
 */
IR_message_rx_t module_IR_message_rx = IR_message_rx_dummy;

/**
 * @brief IR message successfully sent callback.
 * Registering an empty dummy is necessary before sending IR messages.
 * Otherwise IR messages would make the program fail.
 */
IR_message_tx_success_t module_IR_message_tx_success = IR_message_tx_success_dummy;

// TODO till, add some buffer for sending regular messages
CAN_message_t CAN_buffer_message_tx;  // TODO rename!!!
kilogrid_address_t CAN_buffer_address_tx;
// flag for signaling that the user wants to send a message 
uint8_t send_module_to_module_msg_flag;
uint8_t debug_till_var = 0; 



uint8_t debug_till(){
	return debug_till_var;
}

uint16_t get_random(uint16_t low, uint16_t high){
	return ((rand() % (high-low)) + low);
}

/**** PRIVATE FUNCTIONS ****/

/**
 *  @brief Used internally to send the next message on the CAN message tx buffer. The function is called by the main while(1) loop (see module_start() function) at each iteration (best effort). It is the duty of the user to place CAN messages on the buffer through the add_CAN_message_to_buffer() function.
 *
 *  @return 1 if successful.
 *  @see add_CAN_message_to_buffer
 *
 */
uint8_t send_next_CAN_message(){
	if(!RB_empty(CAN_message_tx_buffer)){
		// TODO: does this work like this!?!
		//debug_till_var = 0;
		//if (!debug_till_var){
		// debug_till_var = CAN_message_tx(&RB_front(CAN_message_tx_buffer), CAN_address_to_dispatcher);
		return CAN_message_tx(&RB_front(CAN_message_tx_buffer), CAN_address_to_dispatcher);
		//	_delay_ms(10);  // is this to long
		//}
		// TODO: this is miss leading because it could be the case that no message was send and you still return 1?!?
		// should be more like 
		// return CAN_message_tx(&RB_front(CAN_message_tx_buffer), CAN_address_to_dispatcher);
		// or do i missunderstand something ?
	}
	return 2;  // error try to send from empty buffer should not occur 
}

// this method should allow you to send some can messages as you want 
void CAN_send_broadcast_message(CAN_message_t* msg){
	init_CAN_message(&CAN_buffer_message_tx);
	CAN_buffer_message_tx = *msg;  // check if we copy the content here!
	//CAN_buffer_address_tx = addr;  // TODO do we copy the content  
	send_module_to_module_msg_flag = 1;
}

void CAN_message_sent(){
	// remove front element of the buffer
}

/**
 * @brief Initialize the UART for data logging through serial. This function assumes a F_CPU clock of 8 MHz.
 *
 */
void init_serial(void){
	SET_AS_OUTPUT(TXD);
	SET_AS_INPUT(RXD);
	UBRR0 = 1; //256000 baud
	UCSR0A = 0;
	UCSR0B |= (1 << TXEN0);
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
}

/**
 * @brief Function that makes the program jump directly to the bootloader section
 * of the FLASH (NRWW region).
 */
void goto_bootloader(){
	//eeprom_write_byte(EEPROM_VALID_APPLICATION_BIT_ADDR, 0x00); // invalidate current application in FLASH
	while(!eeprom_is_ready());

/*	cli();*/

	MCUCR = (1<<IVCE);
	MCUCR = (1<<IVSEL);

	EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;    //disable A/D converter
	TIMSK0 = 0; TIMSK1 = 0; TIMSK2 = 0; TWCR = 0;         //disable timers

	asm volatile ("jmp 0x7000"); // jump to bootloader (NRWW) section
}

/**
 * @brief Send a message to be sent on cell number "c" without delay.
 *
 * @param	m	Message to send.
 * @param	c	Identifier of the cell to send the message from.
 */
void send_IR_message(IR_message_t *m, cell_num_t c){
	cli();
	m->crc = message_crc(m); // compute message CRC
	SELECT_IR_TRANSMITTER(c);
	message_send(m);
	sei();
}


/**
 * @brief Process CAN messages before calling the user callback function.
 * This allows CAN messages to be filtered, to restrict the access of the user.
 * By default, all CAN messages are transferred to the user, except when the CAN message is identified as a special type of message (CAN_message_type_t)
 * to unnecessary CAN messages received.
 * This function takes the CAN message buffer and saves it before any operation on the current CAN message.
 * It has been defined as an "inline" function to avoid the overhead of function calls when called by the CAN ISR.
 *
 * Unknown message types will be handled by module_CAN_message_rx.
 *
 * @note It is forbidden to use blocking calls in this function, as well as the moduleLED.c functionalities (too slow).
 */
static inline void process_CAN_message() {
	uint8_t i, j = 0;

	CAN_msg_type = CAN_message_rx_buffer.data[0];
	uint8_t current_CAN_msg_number = CAN_message_rx_buffer.data[1];

	// transfer the content of the CAN msg buffer into the CAN_message struct to avoid race conditions
	if(CAN_msg_type == CAN_KILO_BOOTPAGE){
		CAN_message_bootpage = CAN_message_rx_buffer;
	}
	else{
		CAN_message_rx = CAN_message_rx_buffer;
	}

	switch(CAN_msg_type){

		case CAN_FORWARD_IR_MESSAGE_STOP:

			init_IR_message(&IR_setup_message);

			disable_IR_tx(CELL_00);
			disable_IR_tx(CELL_01);
			disable_IR_tx(CELL_02);
			disable_IR_tx(CELL_03);

			module_state = MODULE_IDLE;

			break;

		case CAN_KILO_BOOTPAGE_SIZE:
			page_total = CAN_message_rx.data[2]; // store the size of the current Kilobot program
			break;

		case CAN_KILO_BOOTPAGE:
			// bootpage is faulty if the sequence number is not contiguous
			if(current_CAN_msg_number != 0
				&& current_CAN_msg_number != (previous_CAN_msg_number+1))
				{
					bootpage_faulty = 1;
				}

			// if bootpage is faulty then drop the rest of the bootpage
			if(bootpage_faulty) {
				break;
			}

			// current_CAN_msg_number is the bootpage data offset, in bytes
			page_offset = (current_CAN_msg_number * 6);

			if(current_CAN_msg_number < (CAN_MESSAGES_PER_BOOTPAGE-1)){
				for(j = 0; j < 6; j++){
					current_bootpage.data[page_offset + j] = CAN_message_bootpage.data[2 + j];
				}
			}
			else if(current_CAN_msg_number == (CAN_MESSAGES_PER_BOOTPAGE-1)){ // if the message is the last CAN message of current bootpage: only copy the two remaining words (128 - 126 = 2 bytes)
				for(j = 0; j < 2; j++){
					current_bootpage.data[page_offset + j] = CAN_message_bootpage.data[2 + j];
				}
			}

			if(current_CAN_msg_number == (CAN_MESSAGES_PER_BOOTPAGE-1)){ // bootpage reception complete
				page_count++;

				module_state = MODULE_BOOTLOADING_KILOBOTS; // switch to MODULE_BOOTLOADING_KILOBOTS state to send the bootpage in the main loop

				bootpage_sent = 0;

			} // if bootpage received

			previous_CAN_msg_number = current_CAN_msg_number;

			break;

		case CAN_KILO_BOOTPAGE_NUMBER:
			bootpage_faulty = 0;
			previous_CAN_msg_number = 0;
			page_address = CAN_message_rx.data[2]; // store address of current bootpage
			module_state = MODULE_WAITING_FOR_KILOBOT_BOOTPAGE;
			break;

		case CAN_FORWARD_IR_MESSAGE: // IR message to be forwarded: receive it and broadcast to every cell
			if(current_CAN_msg_number == 0){ // first part of the IR message

				// copy CAN message into IR message - TODO change this using mem copy
				// i points to the CAN message data byte
				// this step copies 6 bytes of overall 9 into the setup message data buffer
				for (i = 0; i < 6; i++) {
					IR_setup_message.data[i] = CAN_message_rx.data[i+2];
				}

			}
			else if(current_CAN_msg_number == 1){ // second part of the IR message
				// there are 6 bytes in the IR_setup_message.data buffer
				// remaining IR data bytes to copy from CAN message: 9 - (CAN_MESSAGE_SIZE - 2) = 3
				// start from byte 2 in the CAN message for 3 bytes and copy to the IR_setup_message.data buffer from byte 6
				for (i = 0; i < 3; i++){
					IR_setup_message.data[6+i] = CAN_message_rx.data[i+2];
				}
				// i is incremented and points to the last byte of the IR message, which is the type
				IR_setup_message.type = CAN_message_rx.data[i+2];

				// if this is a calibration message but we are not part of the calibration area, skip processing
				if(IR_setup_message.type == CALIB && module_calib_restricted == 1) {
					if( module_uid_y_coord < module_calib_y_min ||
						module_uid_y_coord > module_calib_y_max ||
						module_uid_x_coord < module_calib_x_min ||
						module_uid_x_coord > module_calib_x_max) {
							return;
					}
				}

				module_state = MODULE_SETUP_KILOBOTS; // will enter the MODULE_SETUP_KILOBOTS state after this ISR (ie. after receiving the entire IR message)

				// display color
				switch(IR_setup_message.type){
					case RESET:
						c = GREEN;
						break;
					case BOOT:
						c = BLUE;
						bootloading_current_cell = CELL_00; // start bootloading on cell CELL_00
						page_count = 0; // reset page count
						break;
					case BOOTPGM_PAGE:
						c = CYAN;
						break;
					case SLEEP:
						c = WHITE;
						break;
					case RUN:
						c = MAGENTA;
						break;
					case VOLTAGE:
						c = YELLOW;
						break;

					// NOTE (ATFER REV 812): THE MODULE SHOULD NOT RECEIVE ANY CAN_FORWARD_IR_MESSAGE CONTAINING A BOOTPGM_SIZE IR MESSAGE TYPE (SENT AS A SEPARATE CAN MESSAGE AT THE BEGINNING OF THE BOOTLOADING PROCEDURE)
					case BOOTPGM_SIZE:
						c = CYAN;
						break;

					case CHARGE:
						c = RED;
						break;

					case CALIB:
					case GO_STRAIGHT:
					case TURN_LEFT:
					case TURN_RIGHT:
						c = YELLOW;
						break;

					default:
						c = LED_OFF;
						break;
				}

			}

			break;

		case CAN_MODULE_BOOT:
			goto_bootloader();
			break;

		case CAN_MODULE_IDLE:
			module_state = MODULE_IDLE;
			break;

		case CAN_MODULE_SETUP:
			module_state = MODULE_SETUP;
			break;

		case CAN_MODULE_RUN:
			module_state = MODULE_RUNNING;
			break;

		case CAN_MODULE_CONFIG_SIZE:
			// size of the configuration data in bytes
			configuration_size = CAN_message_rx.data[2];

			// number of CAN messages
			configuration_total = CAN_message_rx.data[2] / 6;
			if(CAN_message_rx.data[2] % 6 != 0) {
				configuration_total += 1;
			}

			configuration_faulty = 0;
			configuration_msg_counter = 0;
			module_state = MODULE_WAITING_FOR_CONFIGURATION_DATA;
			c = RED;

			break;

		case CAN_MODULE_CONFIG_TRANSFER:
			if(module_state != MODULE_WAITING_FOR_CONFIGURATION_DATA ||
			   configuration_faulty){
				return;
			}

			if(current_CAN_msg_number != configuration_msg_counter) {
				configuration_faulty = 1;
				return;
			}

			for(i = 0; i < 6; i++) {
				//multiple messages can address a module, data is collated
				configuration[configuration_msg_counter*6 + i] = CAN_message_rx.data[i+2];
			}

			configuration_msg_counter += 1;

			if(current_CAN_msg_number == configuration_total-1) {
				received_setup = 0;
				c = GREEN;
				module_state = MODULE_SETUP;
			}
			break;

		case CAN_TRACKING_REQ:
			sending_tracking_data = 1;
			sent_tracking_header = 0;
			messages_to_send = RB_size(CAN_message_tx_buffer);
			break;
		default:
			module_CAN_message_rx(&CAN_message_rx); // transfer CAN message to the user. The user can receive all other CAN messages.
			break;
	}
}

/**** FUNCTIONS DEFINITIONS ****/

void module_init(void){

	cli(); // enter critical section - disable interrupts

	// init random
	srand(time(NULL));

	has_started = 0;
	received_setup = 0;

	module_state = MODULE_INIT;

	/* Peripherals */
	// disable watchdog
	//WDTCSR |= (1<<WDCE)|(1<<WDE); // re-enable if not working
	WDTCSR = 0;

	// Reset all MCU ports
	ports_off();

	module_uid_x_coord = eeprom_read_byte((uint8_t *)MODULE_EEPROM_X_COORD_ADDR); // read the x coordinate on the grid (LSB of module_uid)
	module_uid_y_coord = eeprom_read_byte((uint8_t *)MODULE_EEPROM_Y_COORD_ADDR); // read the y coordinate on the grid (MSB of module_uid)
	module_uid = module_uid_x_coord | ((uint16_t)module_uid_y_coord << 8); // merge the two in the module_uid variable

	module_calib_restricted = eeprom_read_byte((uint8_t *)MODULE_EEPROM_CALIB_RESTRICTED);
	module_calib_x_min = eeprom_read_byte((uint8_t *)MODULE_EEPROM_CALIB_X_MIN);
	module_calib_x_max = eeprom_read_byte((uint8_t *)MODULE_EEPROM_CALIB_X_MAX);
	module_calib_y_min = eeprom_read_byte((uint8_t *)MODULE_EEPROM_CALIB_Y_MIN);
	module_calib_y_max = eeprom_read_byte((uint8_t *)MODULE_EEPROM_CALIB_Y_MAX);

	// initialized reused address descriptor in the direction of the dispatcher
	CAN_address_to_dispatcher.x = module_uid_x_coord;
	CAN_address_to_dispatcher.y = module_uid_y_coord;
	CAN_address_to_dispatcher.type = ADDR_DISPATCHER;

	// init address for module broadcast 
	CAN_address_to_modules.x = 0;
	CAN_address_to_modules.y = 0;
	CAN_address_to_modules.type = ADDR_BROADCAST_TO_MODULE;  // needed in order to generate 

	init_CAN_message(&poll_response_message);
	poll_response_message.data[0] = CAN_TRACKING_KILOBOT_START;
	poll_response_message.id =  module_uid_x_coord + (module_uid_y_coord * 20) + 1000;  // this should range from 1 to 200; 0 is the dispatcher -> just number the module numbers 

	RB_init(CAN_message_tx_buffer); // init CAN message tx buffer

	module_CAN_message_tx_success = CAN_message_sent; // register function callback  - TODO is this a problem? calls empty method which should remove the first element of the buffer????

	// Setup Analog Comparator (enable IR reception) and ADC (distance measurements)
	ACOMP_SETUP();
	ADC_SETUP();

	// Setup peripherals
	init_serial();
	init_module_LED();
	init_ModuleCAN(module_uid_x_coord, module_uid_y_coord);
	init_module_IR();

	brightness_dir = 1; // increasing
	configuration_size = 0;

	#ifdef ITF
		// do not display animation. This will be part of the test routines in the main loop

		_delay_ms(10000); // waiting time before the test begins

		module_state = MODULE_RUNNING; // directly go to the main!

	#else
		// display animation
		for(uint8_t j = 0; j < 3; j++){ // repeat 3 times

			// light up all LEDs in sequence
			for(uint8_t i = 0; i < 4; i++){
				set_LED(i, 1 << j);
				_delay_ms(50);
			}
			// switch off all LEDs in sequence
			for(uint8_t i = 0; i < 4; i++){
				set_LED(i, LED_OFF);
				_delay_ms(50);
			}
		}
	#endif

	uint8_t i;
	for(i=0; i<MODULE_CONFIGURATION_BUFFER_SIZE; i++) {
		configuration[i] = 0;
	}

	// init message buffer 
    //CAN_buffer_message_tx* = NULL;
    init_CAN_message(&CAN_buffer_message_tx); 
    // init flag to 0, as we do not want to send 
    send_module_to_module_msg_flag = 0;

	cprints("Module initialized.");

	sei(); // exit critical section - enable interrupts
}

void module_start(void (*setup)(void), void (*loop)(void)) {
	uint8_t i, j;

	#ifdef ITF // force setup() once before executing main loop
		setup();
	#endif

	/**** MODULE MAIN LOOP AND INTERNAL STATE MACHINE ****/
	while(1){

		//#define ULTRA_VERBOSE_ON
		#ifdef ULTRA_VERBOSE_ON
			cprinti(module_state);
		#endif

		switch(module_state){
			case MODULE_INIT:
				module_state = MODULE_IDLE; // initial state after module initialization

				if(module_autostart) {
					module_state = MODULE_SETUP;
				}
				break;

			case MODULE_IDLE:
				_delay_ms(70);

				if(brightness_idle == 20){
					brightness_dir = -1;
				}
				else if(brightness_idle == 0){
					brightness_dir = 1;
				}

				brightness_idle += brightness_dir;

				set_all_LEDs_with_brightness(WHITE, brightness_idle);

				break; // wait for commands to arrive

			case MODULE_SETUP_KILOBOTS:

				send_IR_message(&IR_setup_message, CELL_00);
				send_IR_message(&IR_setup_message, CELL_01);
				send_IR_message(&IR_setup_message, CELL_02);
				send_IR_message(&IR_setup_message, CELL_03);

				set_all_LEDs(c); // display state

				break;

			case MODULE_WAITING_FOR_KILOBOT_BOOTPAGE:
			case MODULE_WAITING_FOR_CONFIGURATION_DATA:
				break;

			case MODULE_BOOTLOADING_KILOBOTS:

				if(bootpage_sent == 0) {

					cli(); // enter critical section

					// tests
					#ifdef VERBOSE_ON
					// test: page_address == 0 at some point here?
					cprints("page_addr");
					cprinti(page_address);
					cprints(""); // line return
					#endif

					#ifdef DUMP_BOOTPAGE_ON
					for(i = 0; i < 128; i++){
						cprinti(current_bootpage.data[i]);
					}
					cprints("");
					cprints("");
					#endif

					// Before sending the program, the size of the program is sent to the Kilobot
					if(page_count == 1){ // the first bootpage has been received
						IR_setup_message.type = BOOTPGM_SIZE;
						IR_setup_message.data[0] = page_total;
						send_IR_message(&IR_setup_message, bootloading_current_cell); // send IR message directly
						set_all_LEDs(LED_OFF);
					}

					// preparing IR messages
					IR_setup_message.type = BOOTPGM_PAGE;
					IR_setup_message.data[0] = page_address;

					/**** Send bootpage to Kilobot ****/
					for(i = 0; i < BOOTPAGE_SIZE; i += 6){
						IR_setup_message.data[1] = i / 2;
						// copy bootload message payload from current_bootpage data
						for(j = 0; j < 6; j++) {
							IR_setup_message.data[2 + j] = current_bootpage.data[i + j]; // copy bootpage data in IR message struct to be sent
						}
						send_IR_message(&IR_setup_message, bootloading_current_cell); // send IR message directly
					}

					sei(); // exit critical section

					bootpage_sent = 1; // avoids sending the same bootpage twice

					if((page_address & 0x01) == 0){ // even bootpage number
						set_LED(bootloading_current_cell, BLUE);
					}
					else{
						set_LED(bootloading_current_cell, GREEN);
					}

					if(page_count == page_total){ // last bootpage received
						page_count = 0; // simply reset counter - will be set to 1 when receiving the first bootpage of the program again
						set_all_LEDs(LED_OFF);
						bootloading_current_cell = (bootloading_current_cell + 1) % 4; // jump to the next cell
					}
				}

				break;

			case MODULE_SETUP:
				if(!received_setup) {
					received_setup = 1;
					setup();
				}

				if(module_autostart) {
					module_state = MODULE_RUNNING;
				}

				break;

			case MODULE_RUNNING:
				if(!has_started){
					has_started = 1;
					// first, broadcast RUN message to the Kilobots for a little while (we assume that all robots are in reset mode already) TODO needs to be a bit longer imo
					IR_setup_message.type = RUN;
					set_all_LEDs(MAGENTA);

					for(i = 0; i < 10; i++){  // is this a good way to implement it ?
						send_IR_message(&IR_setup_message, CELL_00);
						send_IR_message(&IR_setup_message, CELL_01);
						send_IR_message(&IR_setup_message, CELL_02);
						send_IR_message(&IR_setup_message, CELL_03);
					}

					RB_init(CAN_message_tx_buffer); // throw away any previous messages to be sent
				}

				/**** EXECUTE USER PROGRAM ****/
				debug_till_var = sending_tracking_data;
				if (sending_tracking_data && sent_tracking_header){
					debug_till_var = 3;
				}
				loop(); // execute main loop defined in the main
				break;
			default:
				break;
		}

		// BLOCK FOR SENDING CAN MESSAGES 
		if(sending_tracking_data) {  // TODO: show giovanni
		//if(0) {  // out command because we do not want to send any further tracking messages - work around 
			if(!sent_tracking_header) {
				poll_response_message.data[0] = CAN_TRACKING_KILOBOT_START;
				poll_response_message.data[1] = messages_to_send;
				poll_response_message.header.length = 2;
				
				// if (CAN_message_tx(&poll_response_message, CAN_address_to_dispatcher)==1){
				// 	sent_tracking_header = 1;
				// } else {
				// 	debug_till_var = 2;
				// }
				CAN_message_tx(&poll_response_message, CAN_address_to_dispatcher);
				sent_tracking_header = 1;
				_delay_ms(1);
			}else if(messages_to_send > 0) {
				// IMO we have to catch if message was transmitted successfully
				// if (send_next_CAN_message()==1){ 
				// 	messages_to_send -= 1;
				// 	if(!RB_empty(CAN_message_tx_buffer)){ RB_popfront(CAN_message_tx_buffer); }
				// 	//debug_till_var = 2;
				// 	_delay_ms(1);
				// }
				send_next_CAN_message();
				messages_to_send -= 1;
				RB_popfront(CAN_message_tx_buffer);
				_delay_ms(1);
			}
			else {
				sending_tracking_data = 0;
				poll_debug_led_toggle = !poll_debug_led_toggle;
			}
		}
		else {  // if there is no tracking data to be send, send my stuff, dirty hack i know 
			if (send_module_to_module_msg_flag){
				send_module_to_module_msg_flag = 0;
				CAN_message_tx(&CAN_buffer_message_tx, CAN_address_to_modules);
				_delay_ms(1);
				
			}
		}

		// reset has_started flag if state is not equal to MODULE_RUNNING
		if(module_state != MODULE_RUNNING){
			has_started = 0;
		}

		if(module_state != MODULE_IDLE){
			brightness_idle = 0; // resetting brightness for a better animation rendering (all modules look synchronized after a new command)
		}
	}
}

void module_enable_autostart() {
	module_autostart = 1;
}


inline CAN_message_t* next_CAN_message() {
	CAN_message_t* ret = NULL;

	if(!RB_full(CAN_message_tx_buffer)) {
		ret = &RB_back(CAN_message_tx_buffer);
		RB_pushback(CAN_message_tx_buffer); // move internal pointer to the next element
	}

	return ret;
}

/**
 * @brief Interrupt Service Routine for INT0 (PD2 - nCAN_INT pin).
 * The ISR is triggered by the reception of CAN messages.
 *
 */
#ifndef CAN_INTERRUPT_DISABLE
ISR(INT0_vect) {
	// TODO: I do not know how the interrupts are handled. There are 2 external interrupts in the atmega328p (INT0_vect, INT1_vect)
	//  see https://www.arxterra.com/10-atmega328p-interrupts/ 
	//  In the following is a implementation, which, assuming it is always the same interrupt triggering the same isr, maybe handles 
	//  if we observe an overflow... but it is not tested nor correct, just a starting point. Also I am not sure if these interrupts are 
	//  activated 

	// uint8_t eflg_register = mcp2515_read_register(EFLG);

	// if (BIT_IS_SET(eflg_register,RX0OVR)){ // buffer RXB0 has an overflow 
	// 	// handle overflow 
	// 	mcp2515_bit_modify(CANINTF, (1<<RX0IF), 0);
	// 	mcp2515_bit_modify(EFLG, (1<<RX0OVR), 0);
	// }
	// else if (BIT_IS_SET(eflg_register,RX1OVR)){  // buffer RXB1 has an overflow 
	// 	mcp2515_bit_modify(CANINTF, (1<<RX1IF), 0);
	// 	mcp2515_bit_modify(EFLG, (1<<RX1OVR), 0);
	// }else{  // just received a new can msg
		mcp2515_get_message(&CAN_message_rx_buffer); // retrieve CAN message
		process_CAN_message();
	// }
}
#endif
