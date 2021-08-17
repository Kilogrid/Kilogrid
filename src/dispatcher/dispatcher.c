// TODO credits to Kilolib v2

#include "bootldr.h"
#include "IO.h"
#include "Dispatcher.h"
#include "kilogrid.h"
#include "debug.h"
#include "CAN.h"
#include "serial_packet.h"
#include "stdint.h"
#include "tracking.h"
#include "ringbuffer.h"

#include <avr/io.h>        // for port and register definitions
#include <avr/interrupt.h> // for ISR
#include <util/setbaud.h>
#include <util/delay.h>    // for _delay_ms
#include <string.h>        // for memcpy

volatile uint8_t packet_head = 0;
volatile uint8_t packet_checksum = 0;
serial_packet_raw_t serial_packet;
uint8_t* serial_packet_buffer = (uint8_t*)&serial_packet;
volatile uint8_t has_new_packet = 0;

volatile uint8_t tx_mask = 0;
volatile uint8_t leds_toggle = 0;
volatile uint8_t debug1_toggle = 0;
volatile uint8_t debug2_toggle = 0;
volatile uint8_t fCAN_debug_rx;			// debugging of the CAN
volatile uint8_t fCAN_debug_tx;			// debugging of the CAN

volatile uint8_t fNew_CAN_Msg;
volatile uint8_t fCAN_tx_OK;			// 1 when a message has just be transmitted

volatile uint8_t current_bootload_program_size; // size of the program being currently uploaded

/** @brief true if bootloading the module */
volatile uint8_t module_bootpages = 0;

/** @brief id of bootpage to get next */
volatile uint8_t module_bootpage_number = 0;

/** @brief true if bootloading the module */
volatile uint8_t kilobot_bootpages = 0;

/** @brief id of bootpage to get next */
volatile uint8_t kilobot_bootpage_number = 0;

/** @brief true if the user started the experiment, triggers polling the modules for tracking messages */
volatile uint8_t experiment_running = 0;

/** @brief  */
volatile uint8_t module_messages_total = 0;

/** @brief  */
volatile uint8_t module_messages_received = 0;

/** @brief  */
volatile kilogrid_address_t current_module_address;

volatile uint32_t module_message_send_timer;

volatile uint8_t waiting_for_kilogui = 0;
volatile uint8_t tracking_messages_sent = 0;

volatile uint8_t polling_counter = 0;

uint8_t module_max_x = 0;
uint8_t module_max_y = 0;
const static uint8_t polling_counter_limit = 5; // after this limit, the dispatcher sends a serial packet to KiloGUI to accept incoming commands (the lesser, the more reactive, but the bigger the overhead brought by sending a serial packet)

/** @brief  */
const static uint32_t module_message_send_timeout = 10;

RB_create(CAN_message_tx_buffer, CAN_message_t, 4);

CAN_message_t can_msg;
CAN_message_t dummy_can;
CAN_message_t CAN_message_rx;
uint8_t current_CAN_msg_number;

bootpage_data_bytes_t current_bootpage;

kilogrid_address_t kilogrid_addr; // will be used to forward the information to the modules
volatile uint32_t disp_ticks;

/**
 *  @brief Empty dummy callback for the CAN tx success handling.
 */
void CAN_message_tx_success_dummy(){}
CAN_message_tx_success_t module_CAN_message_tx_success = CAN_message_tx_success_dummy;

void init_serial(void){
//initialize uart for output of serial debugging info
	SET_AS_OUTPUT(TXD);
	SET_AS_INPUT(RXD);
	UBRR0 = UBRR_VALUE; // set as a macro from the setbaud.h lib

	#if USE_2X
	UCSR0A |= (1<<U2X0);
	#else
	UCSR0A &= ~(1<<U2X0);
	#endif

	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);              // No parity, 8 bits comm, 1 stop bit
	UCSR0B |= (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);    // Enable reception, transmission, and reception interrupts
}

void dispatcher_init(void){
	cli();

	init_serial();

	SET_AS_OUTPUT(DEBUG_1);
	SET_AS_OUTPUT(DEBUG_2);

	init_ModuleCAN(0,0); // don't care: the dispatcher does not have a row and column number assigned

	SET_AS_OUTPUT(nCAN_EN1);
	SET_AS_OUTPUT(nCAN_EN2);
	PIN_LOW(nCAN_EN1);
	PIN_LOW(nCAN_EN2);

	disp_ticks = 0;
	disp_timer_setup();

	sei();
}

void CAN_rx(CAN_message_t *m){
	uint8_t type = m->data[0] & 0b00111111;
	uint8_t c = (m->data[0] & 0b11000000) >> 6;
	kilogrid_address_t address = get_coord_from_CAN_ID(m);
	fNew_CAN_Msg = 1;

	switch(type) {
		case CAN_TRACKING_KILOBOT:
			tracking_message_received(address, c, (tracking_user_data_t*)(m->data+1));
			module_messages_received += 1;
			break;

		case CAN_TRACKING_KILOBOT_START:
			module_messages_total = m->data[1] + 1;
			module_messages_received = 1;
			break;
	}

	fCAN_debug_rx = !fCAN_debug_rx;
	if(fCAN_debug_rx){
		PIN_HIGH(DEBUG_2);
	}
	else{
		PIN_LOW(DEBUG_2);
	}
	
	CAN_message_rx = *m; // copy content of received message into internal message struct

}
void CAN_tx_success(uint8_t success){

	fCAN_tx_OK = 1;

	fCAN_debug_tx = !fCAN_debug_tx;
	/*
	if(fCAN_debug_tx){
		PIN_HIGH(DEBUG_1);
	}
	else{
		PIN_LOW(DEBUG_1);
	}
	*/
}

void CAN_send_bootpage(bootpage_data_bytes_t *bootpage, uint8_t bootpage_number){

	CAN_message_t CAN_bp_msg;
	uint8_t CAN_msg_num;
	uint8_t i, j;

	kilogrid_address_t broadcast_addr;

	broadcast_addr.type = ADDR_BROADCAST;

	#ifdef DEBUG_CAN_SEND_BOOTPAGE // test routine for this function

	// test if main memcpy of current function is working properly by creating a dummy bootpage
	bootpage_data_bytes_t dummy_bp;

	// init dummy bp
	for(i = 0; i < BOOTPAGE_SIZE; i++){
		dummy_bp.data[i] = i; // each data byte contains its address in the array
	}

	// make bootpage pointer point to dummy bootpage instead (to be removed)
	bootpage = &dummy_bp;

	#endif // test routine

	init_CAN_message(&CAN_bp_msg);

	// first, send bootpage address
	CAN_bp_msg.data[0] = CAN_KILO_BOOTPAGE_NUMBER;
	CAN_bp_msg.data[1] = 0; // current message number (only one CAN message for this type)
	CAN_bp_msg.data[2] = bootpage_number;

	CAN_message_tx(&CAN_bp_msg, broadcast_addr); // broadcast to all modules

	_delay_ms(10); // leave some time to process message

	CAN_msg_num = 0;

	// send all boot page words through CAN, byte per byte
	for(i = 0; i < BOOTPAGE_SIZE; i += CAN_MESSAGE_PAYLOAD){

		init_CAN_message(&CAN_bp_msg); // useful to distinguish between messages when a message does not fill in all the payload

		CAN_bp_msg.data[0] = CAN_KILO_BOOTPAGE; // CAN message type
		CAN_bp_msg.data[1] = CAN_msg_num; // CAN message number

		if(CAN_msg_num < 21) { // current CAN message is not the last one of a bootpage
			//memcpy(&CAN_bp_msg.data[2], bootpage->data + i, CAN_MESSAGE_PAYLOAD); // copy 6 bytes of bootpage data into CAN message payload
			//#ifdef DEBUG_CAN_SEND_BOOTPAGE
			for(j = 0; j<6; j++){
				CAN_bp_msg.data[2+j] = bootpage->data[i + j];
			}
			//#endif

		}
		else if(CAN_msg_num == 21){ // last message
			//memcpy(&CAN_bp_msg.data[2], bootpage->data + i, 2); // copy 2 bytes of data (128 - 22*6 = 128 - 126 = 2)
			//#ifdef DEBUG_CAN_SEND_BOOTPAGE
			for(j = 0; j<2; j++){
				CAN_bp_msg.data[2+j] = bootpage->data[i + j];
			}
			//#endif
		}

		CAN_message_tx(&CAN_bp_msg, broadcast_addr); // send CAN message to all modules

		CAN_msg_num++; // instead of dividing and computing the message number at each CAN bootpage message, we increment the counter byte

		// visual debug
		leds_toggle = !leds_toggle;
		if (leds_toggle) {
			PIN_HIGH(DEBUG_2);
		}
		else {
			PIN_LOW(DEBUG_2);
		}
		_delay_ms(1); // delay CAN messages - do not go below 500 us (CAN message worst case theoretical duration is 222 us)
		//_delay_ms(10);

	}
}

void send_bootpage_req(serial_packet_type_t type, uint8_t bootpage_number) {
	uint8_t checksum = SERIAL_PACKET_HEADER ^ type ^ bootpage_number;
	
	USART_SEND(SERIAL_PACKET_HEADER);
	USART_SEND(type);
	USART_SEND(bootpage_number);
	USART_SEND(checksum);
}

int main() {
	uint8_t i;
	CAN_message_t* buffered_msg;

	// register function callbacks
	module_CAN_message_rx = CAN_rx;
	//module_CAN_message_tx_success = CAN_tx_success;

	dispatcher_init();
	disp_timer_on();

	/**** VARIABLES INITIALIZATIONS ****/

    current_CAN_msg_number = 0;

	fCAN_debug_rx = 0;
	fCAN_debug_tx = 0;

	init_CAN_message(&can_msg);
	init_CAN_message(&CAN_message_rx);

	RB_init(CAN_message_tx_buffer);

	//cprints("Dispatcher initialized.");

	 // Use LEDs to flash power on indicator signal
	for (i = 0; i < 5; i++) {
        PIN_HIGH(DEBUG_1);
        _delay_ms(200);
        PIN_LOW(DEBUG_1);
        _delay_ms(200);
    }

    // set up address to point to an individual module
    current_module_address.type = ADDR_INDIVIDUAL;
	current_module_address.x = module_max_x;
	current_module_address.y = module_max_y;

	i = 0; // pointer to next serial data to send via CAN (debug)

    while(1) {
		if(experiment_running) {
        	//module finished sending all messages it has
        	if(module_messages_received >= module_messages_total) {
				
				tracking_try_send(0); // no force dump, rely on the trigger
				
				if(current_module_address.x == 0 && 
				   current_module_address.y == 0 &&
				   polling_counter >= polling_counter_limit) {
						polling_counter = 0;
					   
					// send notification to KiloGUI that the dispatcher is available
					send_bootpage_req(SERIAL_PACKET_DISPATCHER_AVAILABLE, 0xFF);
					waiting_for_kilogui = 1;
				}

        		while(!RB_empty(CAN_message_tx_buffer)) {
        			kilogrid_addr.type = ADDR_BROADCAST;
					CAN_message_tx(&RB_front(CAN_message_tx_buffer), kilogrid_addr);
					RB_popfront(CAN_message_tx_buffer);
					_delay_ms(1);
				}

				if(!waiting_for_kilogui) {
					if(current_module_address.x == module_max_x) { // row finished
	        			current_module_address.x = 0;
	        			if(current_module_address.y == module_max_y) { // column finished
	        				current_module_address.y = 0;
							polling_counter += 1;
						}
						else {
	        				current_module_address.y += 1;
						}
					}
	        		else {
	        			current_module_address.x += 1;
	        		}

        			module_messages_total = 1;
					module_messages_received = 0;
					can_msg.data[0] = CAN_TRACKING_REQ;
					can_msg.header.length = 1;
					CAN_message_tx(&can_msg, current_module_address);
					module_message_send_timer = disp_ticks + MS_TO_TICKS(module_message_send_timeout);
        		}					
			}
			/*
			else if(disp_ticks > module_message_send_timer) {
				module_messages_total = 1;
				module_messages_received = 0;
				can_msg.data[0] = CAN_TRACKING_REQ;
				CAN_message_tx(&can_msg, current_module_address);
				module_message_send_timer = disp_ticks + MS_TO_TICKS(module_message_send_timeout);
			}
			*/
        }
        else if(module_messages_received >= module_messages_total) {
        	while(!RB_empty(CAN_message_tx_buffer)) {
        		kilogrid_addr.type = ADDR_BROADCAST;
				CAN_message_tx(&RB_front(CAN_message_tx_buffer), kilogrid_addr);
				RB_popfront(CAN_message_tx_buffer);
				_delay_ms(5);
			}
        }

        if (has_new_packet) { // received another packet on the serial
            has_new_packet = 0; 

            switch(serial_packet.type) {
            	case SERIAL_PACKET_DISPATCHER_CONTINUE:
            		waiting_for_kilogui = 0;
            		break;

				case SERIAL_PACKET_STOP:
					buffered_msg = &RB_back(CAN_message_tx_buffer);
					RB_pushback(CAN_message_tx_buffer);
					init_CAN_message(buffered_msg);

					buffered_msg->data[0] = CAN_FORWARD_IR_MESSAGE_STOP;
					waiting_for_kilogui = 0;
					break;

				/**** FORWARD IR MESSAGE PACKET ****/
				// The IR message is encapsulated in multiple CAN messages since the CAN message payload is too small to carry
				// a single IR message (IR message payload: 11 bytes, CAN message payload (without CAN message number and type): <= 6)
				case SERIAL_PACKET_KILO_FORWARD_IR_MSG:
					buffered_msg = &RB_back(CAN_message_tx_buffer);
					RB_pushback(CAN_message_tx_buffer);
					init_CAN_message(buffered_msg);

					// copy first part of the IR message into a CAN message
					buffered_msg->data[0] = CAN_FORWARD_IR_MESSAGE;
					buffered_msg->data[1] = 0; // CAN message number

					for (i = 0; i < CAN_MESSAGE_SIZE-2; i++) {
						buffered_msg->data[2+i] = serial_packet.data[i];
					}

					// 2nd CAN message

					buffered_msg = &RB_back(CAN_message_tx_buffer);
					RB_pushback(CAN_message_tx_buffer);
					init_CAN_message(buffered_msg);

					buffered_msg->data[0] = CAN_FORWARD_IR_MESSAGE;
					buffered_msg->data[1] = 1; // CAN message number

					for (i = 0; i < 4; i++) {
						buffered_msg->data[2+i] = serial_packet.data[6+i];
					}

					waiting_for_kilogui = 0;
					break;

    			case SERIAL_PACKET_KILO_BOOTLOAD_START:
    				kilobot_bootpages = serial_packet.data[0];
    				kilobot_bootpage_number = 0;

					init_CAN_message(&can_msg);
    				//send program size to kilobot
    				// forward program size to the modules
					can_msg.data[0] = CAN_KILO_BOOTPAGE_SIZE;
					can_msg.data[1] = 0; // CAN message number
					can_msg.data[2] = kilobot_bootpages;
					
					kilogrid_addr.type = ADDR_BROADCAST; // default: broadcast message to all modules
					
					CAN_message_tx(&can_msg, kilogrid_addr);

    				//request the first bootpage
    				send_bootpage_req(SERIAL_PACKET_KILO_BOOTLOAD_GET_PAGE, kilobot_bootpage_number);
					
					PIN_HIGH(DEBUG_2);
					_delay_ms(10);
					PIN_LOW(DEBUG_2);
					_delay_ms(10);
    				break;

				case SERIAL_PACKET_KILO_BOOTPAGE:
					// safe programming: not allowing for segmentation fault

					// copy serial_packet data into bootpage struct
					//memcpy(current_bootpage.data, serial_packet.data + 1, 128);
					for(i = 0; i < 128; i++){
						current_bootpage.data[i] = serial_packet.data[i+1];
					}

					// test
					//#define DEBUG_CAN_SEND_BOOTPAGE_CALL

					#ifdef DEBUG_CAN_SEND_BOOTPAGE_CALL // test routine for this function

					// test if main memcpy of current function is working properly by creating a dummy bootpage
					bootpage_data_bytes_t dummy_bp;

					// init dummy bp
					for(i = 0; i < BOOTPAGE_SIZE; i++){
						dummy_bp.data[i] = i; // each data byte contains its address in the array
					}

					current_bootpage = dummy_bp;

					#endif // test routine

					CAN_send_bootpage(&current_bootpage, serial_packet.data[0]);
					
					//request next bootpage from KiloGUI
					if(kilobot_bootpage_number < kilobot_bootpages-1) {
						kilobot_bootpage_number += 1;
					}
					else{
						kilobot_bootpage_number = 0;
					}
					
					_delay_ms(100);
					send_bootpage_req(SERIAL_PACKET_KILO_BOOTLOAD_GET_PAGE, kilobot_bootpage_number);
					
					break;

				case SERIAL_PACKET_MODULE_CONFIGURATION:
					init_CAN_message(&can_msg);

					can_msg.data[0] = CAN_MODULE_CONFIG_SIZE;

					can_msg.data[1] = 0; // CAN message number

					// size of the configuration data in bytes
					can_msg.data[2] = serial_packet.data[3];

					// see file kilowindow.cpp, sendKilogridConfiguration() function for the order of the bytes in the packet
					kilogrid_addr.x = serial_packet.data[0];
					kilogrid_addr.y = serial_packet.data[1];
					kilogrid_addr.type = serial_packet.data[2];

					CAN_message_tx(&can_msg, kilogrid_addr); // send the config size message

					_delay_ms(5);

					// determine the number of CAN messages that will be sent
					uint8_t number_of_messages = serial_packet.data[3] / 6;
					if(serial_packet.data[3] % 6 != 0) {
						number_of_messages += 1;
					}

					// counter for data bytes copied
					uint8_t j;

					// send out all the required messages
					for(i = 0; i < number_of_messages; i++) {
						init_CAN_message(&can_msg);
						can_msg.data[0] = CAN_MODULE_CONFIG_TRANSFER;
						can_msg.data[1] = i;

						// copy data
						for(j = 0; j < 6; j++) {
							// third byte of the serial packet is the length in bytes
							can_msg.data[j+2] = serial_packet.data[4 + (i*6) + j];
						}

						CAN_message_tx(&can_msg, kilogrid_addr); // send the config size message

						_delay_ms(5);
					}

					PIN_HIGH(DEBUG_2);
					_delay_ms(10);
					PIN_LOW(DEBUG_2);
					_delay_ms(10);

					break;

				// commands to the arena
				case SERIAL_PACKET_KILOGRID_COMMAND:
					buffered_msg = &RB_back(CAN_message_tx_buffer);
					RB_pushback(CAN_message_tx_buffer);
					init_CAN_message(buffered_msg);

					// first byte of serial packet is the type of command
					switch(serial_packet.data[0]) {
						case KILOGRID_STOP_IR:
							buffered_msg->data[0] = CAN_FORWARD_IR_MESSAGE_STOP;
							break;

						case KILOGRID_IDLE:
							buffered_msg->data[0] = CAN_MODULE_IDLE;
							
							// send second IDLE message for the bootloader
							// this will be ignored by the module, because type > 64
							// only recognized by the module bootloader
							// ugly, but neccessary
							buffered_msg = &RB_back(CAN_message_tx_buffer);
							RB_pushback(CAN_message_tx_buffer);
							init_CAN_message(buffered_msg);
							buffered_msg->data[0] = CAN_MODULE_IDLE + 0x9F;

							if(experiment_running) {
								experiment_running = 0;
								tracking_try_send(1); // force dump of tracking data
							}							
							
							break;

						case KILOGRID_SETUP:
							buffered_msg->data[0] = CAN_MODULE_SETUP;
							break;

						case KILOGRID_RUN_EXP:
							buffered_msg->data[0] = CAN_MODULE_RUN;
							disp_ticks = 0; // initialize dispatcher clock
							experiment_running = 1;
							module_messages_received = 0;
							module_messages_total = 0;
							tracking_messages_sent = 0;
							module_max_x = serial_packet.data[1] - 1;
							module_max_y = serial_packet.data[2] - 1;
							current_module_address.x = module_max_x;
							current_module_address.y = module_max_y;
							tracking_init(serial_packet.data[3]);
							break;

						case KILOGRID_BOOTLOAD:
							buffered_msg->data[0] = CAN_MODULE_BOOT;
							break;

						default:
							break;
					}

					waiting_for_kilogui = 0;
				break;

				case SERIAL_PACKET_MODULE_BOOTLOAD_START:
    				module_bootpages = serial_packet.data[0];
    				module_bootpage_number = 0;

					init_CAN_message(&can_msg);
    				//send program size to kilobot
    				// forward program size to the modules
					can_msg.data[0] = CAN_MODULE_BOOTPAGE_SIZE + 0x9F;
					can_msg.data[1] = 0; // CAN message number
					can_msg.data[2] = module_bootpages;
					
					kilogrid_addr.type = ADDR_BROADCAST; // default: broadcast message to all modules
					
					CAN_message_tx(&can_msg, kilogrid_addr);

					//request the first bootpage
					send_bootpage_req(SERIAL_PACKET_MODULE_BOOTLOAD_GET_PAGE, module_bootpage_number);
					
					if(IS_SET(DEBUG_1)){
						PIN_LOW(DEBUG_1);
					}
					else{
						PIN_HIGH(DEBUG_1);
					}		
					
					break;

				case SERIAL_PACKET_MODULE_BOOTPAGE:
					leds_toggle = !leds_toggle;
					if (leds_toggle) {
						PIN_HIGH(DEBUG_2);
					}
					else {
						PIN_LOW(DEBUG_2);
					}

					init_CAN_message(&can_msg);
					// send the CAN message indicating the number of bootpage being sent to the module
    				can_msg.data[0] = CAN_MODULE_BOOTPAGE_NUMBER + 0x9F;
					can_msg.data[1] = 0; // message number
					can_msg.data[2] = module_bootpage_number;

					kilogrid_addr.type = ADDR_BROADCAST;

					CAN_message_tx(&can_msg, kilogrid_addr);

					current_CAN_msg_number = 0;

					// send all boot page words through CAN, byte per byte
					for(i = 0; i < BOOTPAGE_SIZE; i += CAN_MESSAGE_PAYLOAD) {

						init_CAN_message(&can_msg); // useful to distinguish between messages when a message does not fill in all the payload

						can_msg.data[0] = CAN_MODULE_BOOTPAGE + 0x9F; // CAN message type
						can_msg.data[1] = current_CAN_msg_number; // CAN message number

						if(can_msg.data[1] == CAN_MESSAGES_PER_BOOTPAGE-1){  // if the message is the last CAN message of current bootpage: only copy the two remaining words (128 - 126 = 2 bytes)
							memcpy(&can_msg.data[2], serial_packet.data + i + 1, 3);
						}
						else{ // normal case
							// copy bytes from serial data field to CAN message payload
							memcpy(&can_msg.data[2], serial_packet.data + i + 1, CAN_MESSAGE_PAYLOAD);
						}

						kilogrid_addr.type = ADDR_BROADCAST;

						CAN_message_tx(&can_msg, kilogrid_addr);

						current_CAN_msg_number++; // instead of dividing and computing the message number at each CAN bootpage message, we increment the counter byte

						// visual debug
						leds_toggle = !leds_toggle;
						if (leds_toggle) {
							PIN_HIGH(DEBUG_2);
						}
						else {
							PIN_LOW(DEBUG_2);
						}
						_delay_ms(1); // delay CAN messages - do not go below 500 us (CAN message worst case theoretical duration is 222 us)

					}

					// if there are more bootpages, request them
					if(module_bootpage_number < module_bootpages-1) {
						module_bootpage_number += 1;
						send_bootpage_req(SERIAL_PACKET_MODULE_BOOTLOAD_GET_PAGE, module_bootpage_number);
					}
					// else{
						// module_bootpage_number = 0;
					// }

					break;

				default:
					break;
            } // switch serial packet type
        } // if has new packet
		
    } // while 1

    return 0;
}

/**
 * @brief Interrupt service routine for incoming serial packets of data.
 * Each incoming byte is stored in the UDR0 register and saved at the beginning of the ISR.
 * The main function implements the actions to take for each packet type.
 *
 */
 ISR(USART_RX_vect) {
    uint8_t rx = UDR0;

    serial_packet_buffer[packet_head] = rx; // store received byte in packet buffer
    packet_checksum ^= rx;
    packet_head++;

    if (packet_head > 1) {
    	if(serial_packet_buffer[1] == SERIAL_PACKET_DISPATCHER_CONTINUE && packet_head == 3) {
        	packet_head = 0;
    	}
    	else if(packet_head == SERIAL_PACKET_SIZE) {
    		packet_head = 0;
    	}
    }

    if (packet_head == 0) {
        if (packet_checksum == 0) { // if no errors detected
            has_new_packet = 1; // signify new arrival of packet to main loop
        }
		else{
			packet_checksum = 0;
		}
    }
}


/**
 * @brief Interrupt Service Routine for INT0 (PD2 - nCAN_INT pin)
 *
 */
ISR(INT0_vect) {
	// get message from the CAN controller and pass its address to CAN_message_rx
	mcp2515_get_message(&can_msg);

	// call callback function and pass message to the main
	module_CAN_message_rx(&can_msg);
}

ISR(TIMER0_COMPA_vect) {
	disp_ticks++;
}
