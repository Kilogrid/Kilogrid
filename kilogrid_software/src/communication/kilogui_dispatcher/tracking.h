#ifndef __TRACKING_H__
#define __TRACKING_H__

#include "CAN.h"
#include "stdint.h"
#include "serial_packet.h"
#include "kilogrid.h"

/**** DEFINES ****/

/** @brief Number of messages in the buffer that triggers message sending to the pc. */
//#define	TRACKING_ITEM_SEND_TRIGGER			30 // Number of tracking items after which the tracking data should be forwarded through the serial
#define TRACKING_ITEM_BUFFER_SIZE 			40 // Note: the size of the buffer can affect the performance of the program

//#define DEBUG_TRACKING // TO BE REMOVED - fills in the buffer with dummy tracking data
//#define DEBUG_CAN_BUFFERING // TO BE REMOVED - fills in the buffer with dummy CAN messages *after the reception of a CAN message*


/**** TYPEDEFS AND ENUMS ****/


// PATCH #2
#ifdef QT_COMPILATION
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
#endif

typedef struct __attribute__ ((__packed__)) {
  /** @brief */
  kilogrid_address_t	module_address;

  uint8_t				c;

  /** @brief */
  uint8_t  		  		module_data[TRACKING_MSG_USER_DATA];

  /** @brief */
  uint32_t				timestamp;
} tracking_item_t;

#ifdef QT_COMPILATION
#pragma pack(pop)  /* re-introduce original alignment */
#endif

/**
 *  @brief Format of the serial packet containing tracking data.
 *  The header and checksum are the same as a serial_packet_raw_t struct.
 */
typedef struct __attribute__ ((__packed__)) {
	uint8_t header;
	volatile serial_packet_type_t type;
	uint8_t number_of_items;

	tracking_item_t items[TRACKING_ITEM_BUFFER_SIZE];
	
	uint8_t checksum;
	
} serial_packet_tracking_t;

/**** FUNCTIONS PROTOTYPES ****/

void tracking_message_received(kilogrid_address_t address, uint8_t c, tracking_user_data_t* data);
uint8_t tracking_try_send(uint8_t);
void tracking_init(uint8_t);

#ifdef DEBUG_TRACKING
void test_serial_tracking();
#warning "tracking.c (Dispatcher): Tracking in debug mode."
#endif

#endif // __TRACKING_H__
