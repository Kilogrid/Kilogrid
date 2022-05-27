#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>     // delay macros
#include <avr/interrupt.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "module.h"
#include "moduleLED.h"
#include "moduleIR.h"
#include "CAN.h"
#include "mcp2515.h"

#include "../communication.h"

// #define MAX_MESSAGE_REPEAT 1  // defines how often a message is maximum repeated - if in the mean time a other message arrives the timer is resetted 
#define MAX_RESET_TIMER 500

typedef enum{
    false = 0,
    true = 1,
} bool;

// cell utilities 
volatile uint8_t cell_x[4] = {0, 0, 0, 0};
volatile uint8_t cell_y[4] = {0, 0, 0, 0};
volatile uint8_t cell_role[4] = {0, 0, 0, 0};
volatile uint8_t cell_colour[4] = {0, 0, 0, 0};

cell_num_t cell_id[4] = {CELL_00, CELL_01, CELL_02, CELL_03};


// CAN_message_t test_message;  // for structure of a can msg see communication/mcp2515.h

// iterators .. init here to see if we exceed storage limits!
uint8_t cell_it;

// sending data 
IR_message_t IR_message_tx[4];
IR_message_t IR_msg_rx;

tracking_user_data_t tracking_data;


/*-----------------------------------------------------------------------------------------------*/
/* messaging functions                                                                           */
/*-----------------------------------------------------------------------------------------------*/
void IR_rx(IR_message_t *m, cell_num_t c, distance_measurement_t *d, uint8_t CRC_error) {
    // forward tracking message 
    if(!CRC_error && m->type == TRACKING){
        CAN_message_t* msg = next_CAN_message();
        tracking_user_data_t usr_data; 
        usr_data.byte[0] = m->data[0];
        usr_data.byte[1] = m->data[1];
        usr_data.byte[2] = m->data[2];
        usr_data.byte[3] = m->data[3];
        usr_data.byte[4] = m->data[4];
        usr_data.byte[5] = m->data[5]; 
        usr_data.byte[6] = m->data[6];
        if(msg != NULL) { // if the buffer is not full
            serialize_tracking_message(msg, c, &usr_data);
        }
    }
}


// called when pressing setup in the kilogui
void setup(){
    cell_x[0] = (configuration[0] * 2);
    cell_x[1] = (configuration[0] * 2 + 1);
    cell_x[2] = (configuration[0] * 2);
    cell_x[3] = (configuration[0] * 2 + 1);

    cell_y[0] = (configuration[1] * 2 + 1);
    cell_y[1] = (configuration[1] * 2 + 1);
    cell_y[2] = (configuration[1] * 2);
    cell_y[3] = (configuration[1] * 2);

    cell_role[0] = (configuration[2]);
    cell_role[1] = (configuration[3]);
    cell_role[2] = (configuration[4]);
    cell_role[3] = (configuration[5]);

    cell_colour[0] = configuration[6];
    cell_colour[1] = configuration[7];
    cell_colour[2] = configuration[8];
    cell_colour[3] = configuration[9];

    // set colour imo only needed for debugging 
    for(cell_it = 0; cell_it < 4; cell_it++){
        //current_colour[cell_it] = debug_till();
        switch(cell_colour[cell_it]){
            case 0:
                set_LED_with_brightness(cell_id[cell_it], LED_OFF, LOW);
                break;
            case 1:
                set_LED_with_brightness(cell_id[cell_it], BLUE, LOW);
                break;
            case 2:
                set_LED_with_brightness(cell_id[cell_it], YELLOW, LOW);
                break;
            case 3:
                set_LED_with_brightness(cell_id[cell_it], MAGENTA, LOW);
                break;
            default:
                set_LED_with_brightness(cell_id[cell_it], WHITE, LOW);
                break;
        }
    }


}


void loop() {

    // send ir message - position information and sensor information
    for(cell_it = 0; cell_it < 4; cell_it++){
        IR_message_tx[cell_it].type = 11;
        IR_message_tx[cell_it].crc = 0;
        IR_message_tx[cell_it].data[0] = cell_x[cell_it];
        IR_message_tx[cell_it].data[1] = cell_y[cell_it];
        IR_message_tx[cell_it].data[2] = cell_colour[cell_it];
        IR_message_tx[cell_it].data[3] = cell_role[cell_it];
        
        set_IR_message(&IR_message_tx[cell_it], cell_id[cell_it]);
    }


    // set colour
    for(cell_it = 0; cell_it < 4; cell_it++){
    	//current_colour[cell_it] = debug_till();
    	switch(cell_colour[cell_it]){
            case 0:
                set_LED_with_brightness(cell_id[cell_it], LED_OFF, LOW);
                break;
            case 1:
                set_LED_with_brightness(cell_id[cell_it], BLUE, LOW);
                break;
            case 2:
                set_LED_with_brightness(cell_id[cell_it], YELLOW, LOW);
                break;
            case 3:
                set_LED_with_brightness(cell_id[cell_it], MAGENTA, LOW);
                break;
            default:
                set_LED_with_brightness(cell_id[cell_it], WHITE, LOW);
                break;
        }
    }

    _delay_ms(10);
}



int main() {
    module_init();

    // register function callbacks
    //module_CAN_message_rx = CAN_rx;

    //module_CAN_message_tx_success = CAN_message_tx_success_my;
    //module_CAN_message_tx_success = CAN_tx_success;  // seems not to work

    module_IR_message_rx = IR_rx;

    module_start(setup, loop);

    return 0;
}
