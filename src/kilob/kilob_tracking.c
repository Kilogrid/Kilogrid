#include <stddef.h>
#include <stdint.h>
#include "kilob_tracking.h"
#include "kilob_common.h"
#include "kilob_messaging.h"
#include "kilolib.h"
#include "utils.h"

/*** Tracking constants ****/
static const uint32_t TR_TIMEOUT_MS	= 2000;

/** @brief Timer used by the tracking behavior. */
static uint32_t kilob_tr_timer	= 0;  // internal linkage

/**
 * @brief Initialize tracking timer
 */
void kilob_tracking_init()
{
    kilob_tr_timer = 0;
    kilob_messaging_init();
}

/**
 * @brief This function implements the tracking functionality. It sends a message
 * to the cell every TR_TIMEOUT milliseconds. If the message buffer is full, it
 * will cancel the current message.
 */
inline void kilob_tracking(tracking_user_data_t *data) {
	if(kilo_ticks > kilob_tr_timer) {
		kilob_tr_timer = kilo_ticks + MS_TO_TICKS(TR_TIMEOUT_MS);

		IR_message_t* msg = kilob_message_send();
		if(msg == NULL) return;

		// sending user data
		for(uint8_t i = 0; i < TRACKING_MSG_USER_DATA; i++) {
			msg->data[i] = data->byte[i];
		}

		msg->type = TRACKING_MSG_TYPE;
	}
}
