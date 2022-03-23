/** @file CellMain.c */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>  // required if you need delay macro

#include <CAN.h>
#include <debug.h>
#include <module.h>
#include <moduleIR.h>
#include <moduleLED.h>


/**
 * Basic tacking data handler
 *
 * @param m  the tracking message
 * @param c  the cell which receives the message
 */
void send_tracking(IR_message_t *m, cell_num_t c)
{
    // get CAN message buffer
    CAN_message_t *msg = next_CAN_message();

    // send tracking message unless the buffer is full
    if(msg != NULL) {
        serialize_tracking_message(msg, c, (tracking_user_data_t*)m->data);
    }
}

// DEBUG feature/can_broadcast_between_modules: test broadcast between modules
void send_message_to_modules(){
	CAN_message_t *msg = next_CAN_message_to_modules();
	
	msg->type = CAN_MODULE_DATA; // we put it explicitly here for debugging purposes (maybe through external debugger)
	
	// modify message data
	msg->data[0] = 0xAB; // magic number to recognize this message
	
}


/**
 * Callback function called when IR message is received
 *
 * @param m  the message
 * @param c  the cell which receives the message
 * @param d  the estimated distance between the cell and the message sender
 * @param CRC_error  the error flag
 */
void IR_rx(IR_message_t *m, cell_num_t c, distance_measurement_t *d,
           uint8_t CRC_error)
{
    /* Basic implementation */
    if(CRC_error) return;       // ignore message with error

    if(m->type == TRACKING) {   // handle tracking message
        send_tracking(m, c);
    }

    /* [TODO] Put your message handling code */
}

/**
 * Callback function called when a CAN message is received.
 * It is forbidden to put blocking calls or LEDs functionalities in this function (fast code only).
 *
 * @param m  Received CAN message
 * Also see CAN.h.
 */
CAN_message_rx_t CAN_rx(CAN_message_t *m){
	if(m->data[0] == 0xAB){
		// do something
	}
}

/**
 * Setup module controller
 */
void setup()
{
    /* [TODO] Put your setup code */
}


/**
 * Main loop of module controller
 */
void loop()
{
    /* [TODO] Put your experiment code */
}


int main() {
    // [required] initialize module
    // This function should be called first
    module_init();

    // [option] register message receiving callback
    // This code should be put before calling module_start
    module_IR_message_rx = IR_rx;
	module_CAN_message_rx = CAN_rx;

    // [required] start module controller
    module_start(setup, loop);
    return 0;
}
