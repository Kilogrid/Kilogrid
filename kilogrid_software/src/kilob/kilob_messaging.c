#include <stddef.h>
#include "kilob_messaging.h"
#include "kilolib.h"
#include "ringbuffer.h"

#define MESSAGE_BUFFER_SIZE	3

/** @brief The buffer, which contains the messages to be sent. */
RB_create(kilob_message_buffer, IR_message_t, MESSAGE_BUFFER_SIZE);

static uint8_t kilob_messaging_initialized = 0;

// Callbacks supplied to kilolib
IR_message_t * kilob_message_next();
void kilob_message_sent();

void kilob_messaging_init() {
	if(!kilob_messaging_initialized) {
		kilob_messaging_initialized = 1;

		kilo_message_tx = kilob_message_next;
		kilo_message_tx_success = kilob_message_sent;
	}
        RB_init(kilob_message_buffer);
}

/**
 * @brief Internal function called when there is an opportunity to send a
 * message. Messages are sent in an FCFS manner, retrying forever if there are
 * collisions detected by the sender.
 */
inline IR_message_t * kilob_message_next() {
	if(!RB_empty(kilob_message_buffer)) {
		IR_message_t* msg = &RB_front(kilob_message_buffer);
		msg->crc = message_crc(msg);

		return msg;
	}

	return NULL;
}

/**
 * @brief This is the public interface for the message management implemented in
 * kilo behaviors. It returns a pointer to a message object that can be filled
 * with data and type information. The message is scheduled automatically at the
 * end of the message queue, crc is calculated before sending.
 */
inline IR_message_t* kilob_message_send() {
	if(!RB_full(kilob_message_buffer)) {
		RB_pushback(kilob_message_buffer); // "free" space for the new message to be placed in the ring buffer
		return &RB_back(kilob_message_buffer); // return pointer to the message to be filled by the user
	}
	else {
		return NULL;
	}
}

/**
 * @brief Internal function for managing the message buffer, called when a
 * message was sent successfully. This function is assigned to the message_tx_success callback of kilolib_v2.
 */
void kilob_message_sent() {
	RB_popfront(kilob_message_buffer); // remove the front element of the buffer
}
