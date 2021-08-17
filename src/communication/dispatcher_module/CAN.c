// TODO header
/**** INCLUDES ****/

#include <avr/interrupt.h>

#include "CAN.h"
#include "mcp2515.h"
#include "mcp2515_defs.h"

#include "stdint.h"
#include "stdlib.h"
#include "debug.h"
#include "utils.h"
#include "kilogrid.h"

#include <string.h>						// for memcpy


CAN_message_rx_t module_CAN_message_rx;
CAN_message_tx_success_t module_CAN_message_tx_success;
CAN_message_t CAN_rx_message;

/**** FUNCTION POINTERS DECLARATIONS ****/

/**** PRIVATE FUNCTIONS ****/
/**
 * @brief Compute CAN ID based on the type of addressing, destination type (type of node to reach) and coordinates of the CAN node in Kilogrid.
 * The ID field of a CAN message encodes the coordinates of the module in Kilogrid in the following manner: ID<10:0> = [y<4:0> + 1][x<4:0> + 1][destination_type_t]
 * For example, when accessing module with coordinates (x = 0, y = 0) with individual addressing, ID<10:0> = 00001 00001 0.
 * Zeros in the ID field are reserved for broadcasting to all modules (00000 00000 0), modules in a row (yyyyy 00000 0), or modules in a column (00000 xxxxx 0).
 * @param m
 * @param coord
 * @param dest_type
 */
uint16_t get_CAN_ID_from_Kilogrid_address(kilogrid_address_t coord, destination_type_t dest_type){
	uint16_t ID_LSB = 0;
	uint16_t ID_MSB = 0;
	uint16_t CAN_ID = 0;

	dest_type &= 0x01; // destination type can be 0 or 1 (CAN implementation)
	coord.x &= 0b11111; // ensure that the coordinates are encoded in 5 bits (CAN implementation)
	coord.y &= 0b11111; // ensure that the coordinates are encoded in 5 bits (CAN implementation)

	// coordinate guards: coord.x and coord.y can be of maximum (2^5 - 1) - 1 = 30 (coords are incremented by 1 when translated into a CAN ID)
	if( (coord.x + 1) >= 0b11111){
		coord.x = 0b11110; // max address
	}
	if( (coord.y + 1) >= 0b11111){
		coord.y = 0b11110; // max address
	}

	switch(coord.type){
		case ADDR_BROADCAST: // we ignore x and y, and overwrite with 0's --> maximum priority, received by all nodes
			ID_LSB = 0;
			ID_MSB = 0;
			break;
		case ADDR_ROW:
			ID_LSB = 0; // all columns: x coordinate ignored
			ID_MSB = coord.y + 1; // y coordinate increased, so that y = 0 is reserved to column broadcast messages, and individual modules with coord_y = 0 correspond to an index of 1 in the CAN ID
			break;
		case ADDR_COLUMN:
			ID_LSB = coord.x + 1;
			ID_MSB = 0; // y ignored, broadcast to all rows for column x
			break;

		// Individual addressing and messages to the dispatcher are processed in the same way. When a modules wants to send a message to the dispatcher, it appends its x and y coordinates to the message to be sent. Only the dest_type differs (appended at the LSB of the CAN_ID)
		case ADDR_INDIVIDUAL:
		case ADDR_DISPATCHER:
			ID_LSB = coord.x + 1;
			ID_MSB = coord.y + 1; // By default, all coordinates are incremented by 1 before translation into a CAN_ID (x = 0, y = 0 --> CAN_ID = 00001 00001 x)
			break;

		default:
			break;
	}

	CAN_ID = (ID_MSB << 6) | (ID_LSB << 1) | dest_type; // ID format: [y<4:0> + 1][x<4:0> + 1][dest_type]

	return CAN_ID;
}

/**** FUNCTION DEFINITIONS ****/

void init_CAN_message(CAN_message_t *m){
	uint8_t d;

	m->header.rtr = 0;
	m->header.length = 8;
	for(d = 0; d < 8; d++) m->data[d] = 0; // initialize all data bytes
}

void init_ModuleCAN(uint8_t x_coord, uint8_t y_coord){

	// variables
	init_CAN_message(&CAN_rx_message);

	// peripherals
	// set up CAN interrupt
	//cli(); // turn off interrupts

	#ifndef CAN_INTERRUPT_DISABLE
	//EICRA |= (1 << ISC00);    // set INT0 to trigger on any logic change
	EICRA = 0; // interrupt triggered by a low level on INT0 and INT1
	EIMSK |= (1 << INT0);     // Turns on INT0
	#endif

	//sei(); // turn on interrupts

	if(!init_mcp2515(CANSPEED_250)){
		cprints("ERROR: CAN could not initialize.");
	}

	// set filters for current device
	#ifdef MODULE

		kilogrid_address_t module_address;
		module_address.x = x_coord;
		module_address.y = y_coord;

		mcp2515_set_mask(0, 0x7FF);
		mcp2515_set_mask(1, 0x7FF); // compare all bits of received message ID to the filter bits (no mask filtering)

		// set filters for every type of addressing
  		for(uint8_t k = ADDR_BROADCAST; k <= ADDR_INDIVIDUAL; k++){
			module_address.type = k;
			mcp2515_set_filter(k, get_CAN_ID_from_Kilogrid_address(module_address, TO_MODULE) ); // receive messages to be sent to the modules, on the basis of the coordinates of current module

		}

		// setting filters 4 and 5 as well to a valid ID (not used, but have to be set properly)
		module_address.type = ADDR_BROADCAST;
		mcp2515_set_filter(4, get_CAN_ID_from_Kilogrid_address(module_address, TO_MODULE) ); // receive messages to be sent to the modules, on the basis of the coordinates of current modules

		module_address.type = ADDR_BROADCAST;
		mcp2515_set_filter(5, get_CAN_ID_from_Kilogrid_address(module_address, TO_MODULE) ); // receive messages to be sent to the modules, on the basis of the coordinates of current modules

	#elif defined(DISPATCHER)

		// Note: does not seem to filter out addresses to the attention of the modules.

 		kilogrid_address_t disp_address;

		disp_address.type = ADDR_BROADCAST; // 0's in x and y fields

		mcp2515_set_mask(0, TO_DISPATCHER); // sets LSB of mask to 1 --> accept all messages which ID LSB is 1
		mcp2515_set_mask(1, TO_DISPATCHER); // sets LSB of mask to 1 --> accept all messages which ID LSB is 1

		// setup the 6 filters to the dispatcher address (no filter should be left zero!!!)
		for(uint8_t k = 0; k <= 5; k++){
			mcp2515_set_filter(k, get_CAN_ID_from_Kilogrid_address(disp_address, TO_DISPATCHER)); // set filter for buffer 0 (filter RXF0): only last bit
		}

	#endif

}

/**
 *  @brief This function modifies the ID field of the CAN message to be sent, according to the Kilogrid addressing scheme.
 *
 *  @param [in] message Message to be sent
 *  @param [in] dest    Address in Kilogrid
 *  @return Returns 1 if succeeded.
 *
 */
uint8_t CAN_message_tx(CAN_message_t *message, kilogrid_address_t dest) {
	uint8_t msg_success;

	// translate Kilogrid coordinates into CAN ID for current message
	if(dest.type == ADDR_DISPATCHER){
		message->id = get_CAN_ID_from_Kilogrid_address(dest, TO_DISPATCHER);
	}
	else{
		message->id = get_CAN_ID_from_Kilogrid_address(dest, TO_MODULE);
	}

	mcp2515_bit_modify(CANCTRL, (1<<REQOP2) | (1<<REQOP1) | (1<<REQOP0), 0);
	msg_success = mcp2515_send_message(message); // send message and store outcome

	if(msg_success){
		module_CAN_message_tx_success(); // callback
	}

	return msg_success;
}

kilogrid_address_t get_coord_from_CAN_ID(CAN_message_t *m){
	kilogrid_address_t coord;

	coord.x = ((m->id & 0b00000111110) >> 1) - 1; // x --> LSBs (id<5:1>). See get_CAN_ID_from_Kilogrid_address() for conversion from x to id<5:1>.
	coord.y = ((m->id & 0b11111000000) >> 6) - 1; // y --> MSBs (id<10:6>)

	// PATCH #1: retrieve address type correctly

	// special case: the id finishes by 1 --> destination = dispatcher
	if((m->id & 0b00000000001) == TO_DISPATCHER){
		coord.type = ADDR_DISPATCHER;
	}
	else{ // other cases
		if( (m->id & 0b11111111110) == 0){
		coord.type = ADDR_BROADCAST;
		}
		else if( (m->id & 0b00000111110) == 0){
			coord.type = ADDR_COLUMN;
		}
		else if( (m->id & 0b11111000000) == 0){
			coord.type = ADDR_ROW;
		}
		else{
			coord.type = ADDR_INDIVIDUAL;
		}
	}

	return coord;
}

/**
 *  @brief 	Serialize CAN tracking message struct into a CAN message. The data
 *  		contained in the former will be compressed according to the number
 *			of bits required for each field, and assigned to the data field of
 *			the CAN message. Note that the rest of the fields in the data message
 *			(e.g. address field) will remain untouched.
 *
 *  @param [in] m 		Pointer to the CAN tracking message struct
 *  @param [in] CAN_msg Pointer to the CAN message that is the target of the
 *						serialization
 */
void serialize_tracking_message(CAN_message_t* msg, uint8_t cell_id, tracking_user_data_t* data) {
	init_CAN_message(msg);

	cell_id &= 0b00000011;
	msg->data[0] = CAN_TRACKING_KILOBOT | (cell_id << 6);

	// user data fields
	for(uint8_t i = 0; i < TRACKING_MSG_USER_DATA; i++){
		msg->data[1 + i] = data->byte[i];
	}
}
