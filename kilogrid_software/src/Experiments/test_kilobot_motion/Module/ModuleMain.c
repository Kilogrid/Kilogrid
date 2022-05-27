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

cell_num_t cell_id[4] = {CELL_00, CELL_01, CELL_02, CELL_03};
uint8_t cell_it;






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
    
}


void loop() {
     for(cell_it = 0; cell_it < 4; cell_it++){
        set_LED_with_brightness(cell_id[cell_it], WHITE, LOW);
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
