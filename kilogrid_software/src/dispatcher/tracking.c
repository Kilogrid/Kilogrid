#include <stdint.h>
#include "string.h"
#include <avr/interrupt.h> // for ISR

#include "Dispatcher.h"
#include "serial_packet.h"
#include "tracking.h"
#include "debug.h"

volatile uint8_t current_tracking_index	= 0;

volatile uint8_t tracking_item_send_trigger = 0;

/** @brief Serial packet structure that will contain tracking data from the buffer. */
serial_packet_tracking_t serial_packet_tracking;

/**** FUNCTIONS PROTOTYPES (Private functions) ****/
static void send_tracking_data();
void test_serial_tracking();

/**
 * @brief Function that handles the sending of tracking messages to the pc.
 * Serial packets are slow, therefore we only send at certain intervals
 * (PACKET_SEND_TIMEOUT) or when the buffer has TRACKING_ITEM_SEND_TRIGGER messages stored.
 * The reason for the latter is that we cannot predict the rate of the incoming
 * tracking messages precisely and want to minimize the chance of dropping
 * messages because the buffer is full.
 * force_dump: force dump serial packet data if equal to 1
 *
 * First it saves the current state: timestamp and message_counter.
 * Then sends the data back to the pc.
 * To finish up it moves all messages received in the meantime to the beginning
 * of the tracking_items_buffer.
 *
 * Sent message structure is as follows:
 *
 * SERIAL_PACKET_HEADER								1 byte
 * type: SERIAL_PACKET_TRACKING				1 byte
 * total length of the data in bytes  1 byte
 * base timestamp											4 bytes
 * tracking messages									n * sizeof(tracking_item_t)
 * checksum														1 byte
 */
inline uint8_t tracking_try_send(uint8_t force_dump) {
	if(current_tracking_index >= tracking_item_send_trigger || force_dump == 1) {
		
		#ifdef DEBUG_TRACKING
		test_serial_tracking(); // fill in buffer
		#endif

		send_tracking_data();
		current_tracking_index = 0;
		return 1;
	}

	return 0;
}


void tracking_init(uint8_t tracking_item_trigger) {
	tracking_item_send_trigger = tracking_item_trigger;
	current_tracking_index = 0;
	serial_packet_tracking.header = SERIAL_PACKET_HEADER;
	serial_packet_tracking.type = SERIAL_PACKET_TRACKING;
}

/**
 * @brief Called from CAN received interrupt, copies the message to the buffer
 * and sets the address and timestamp fields.
 */
inline void tracking_message_received(kilogrid_address_t address, uint8_t c, tracking_user_data_t* data) {
	for(uint8_t i = 0; i<TRACKING_MSG_USER_DATA; i++) {
		serial_packet_tracking.items[current_tracking_index].module_data[i] = data->byte[i];
	}
	
	serial_packet_tracking.items[current_tracking_index].module_address = address;
	serial_packet_tracking.items[current_tracking_index].c = c;
	serial_packet_tracking.items[current_tracking_index].timestamp = disp_ticks;
	
	current_tracking_index += 1;
}

/**
 *  @brief Used internally to send the serial_packet_tracking struct over the serial.
 *  @param [in] num_item Number of elements to be copied
 *  @see serial_packet_tracking, tracking_items_buffer
 */
static void send_tracking_data(){
	uint16_t i;
	uint8_t* pData = (uint8_t*)&serial_packet_tracking; // pointer to the serial packet to be sent
	
	// init fields
	serial_packet_tracking.number_of_items = current_tracking_index;
	
	uint16_t size = 3 + sizeof(tracking_item_t)*current_tracking_index;
	uint8_t checksum = pData[0] ^ pData[1] ^ pData[2];

	// send the whole packet through the USART, minus the checksum
	for(i = 0; i < size; i++){
		USART_SEND(pData[i]);
		checksum = checksum ^ pData[i];
	}
	
	USART_SEND(checksum);
}

/**
 *  @brief Test filling in buffer and try sending dummy data (verify in KiloGUI).
 *  
 */
void test_serial_tracking(){
	/*
	kilogrid_address_t cell_addr = { .x = 3, .y = 1, .type = ADDR_DISPATCHER};
	
	// CAN_tracking_message_t  cell_dat = {.kilo_uid = 0x15, .subcell_id = 1, .subcell_lgain = 0b0101010101, .subcell_hgain = 0b0111111110}; // direct assignment of bitfields does not appear in memory as such...
	// trying by referencing bitfields as data memory and access it as uint16_t before assignment (needs guards for safety reasons - no memory overlap)
	CAN_tracking_message_t cell_dat_tracking;
	
	cell_dat_tracking.kilo_uid = 0x15; // normal accessing for uint16_t
	cell_dat_tracking.subcell_id = 0b01;
	cell_dat_tracking.subcell_lgain = 0b0101010101;
	//cell_dat_tracking.subcell_hgain = 0b0111111110;
	CAN_message_t cell_dat = tracking_to_CAN_message(&cell_dat_tracking);
	
	// to do: rewrite tracking_item assignment
	
	tracking_item_t new_elem = {.cell_address = cell_addr, .timestamp = 0xFAFAFAFA};
	
	memcpy(new_elem.cell_data, cell_dat.data, 8);
	
	// add new element to the buffer
	RB_pushback(tracking_items_buffer);
	RB_back(tracking_items_buffer) = new_elem;
	*/
}

// PATCH #3
void init_serial_packet_tracking(serial_packet_tracking_t *s){
	uint8_t i,j;

	s->header = 0;
	s->type = 0;
	s->number_of_items = 0;
	
	for(i = 0; i < TRACKING_ITEM_BUFFER_SIZE; i++){
		s->items[i].module_address.x = 0; 
		s->items[i].module_address.y = 0, 
		s->items[i].module_address.type = 0;

		for(j = 0; j<TRACKING_MSG_USER_DATA; j++ ) {
			s->items[i].module_data[j] = 0;
		}
		
		s->items[i].timestamp = 0;
	}	
}
