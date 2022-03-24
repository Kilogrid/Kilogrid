#include <avr/io.h>
#include <avr/wdt.h> 
#include <util/delay.h>     // delay macros
#include <avr/interrupt.h>

#include "module.h"
#include "moduleLED.h"
#include "moduleIR.h"
#include "CAN.h"

#include "../communication.h"

volatile uint8_t cell_colour[4] = {0, 0, 0, 0};
volatile uint8_t cell_msg[4] = {0, 0, 0, 0};
uint8_t colour2[3] = {0x68, 0x4C, 0x75};
cell_num_t cell_id[4] = {CELL_00, CELL_01, CELL_02, CELL_03};

IR_message_t IR_message_tx[4];
IR_message_t IR_msg_rx;

tracking_user_data_t tracking_data;

void IR_rx(IR_message_t *m, cell_num_t c, distance_measurement_t *d, uint8_t CRC_error) {
  if(!CRC_error && m->type == TRACKING){
    CAN_message_t* msg = next_CAN_message();
    if(msg != NULL) { // if the buffer is not full
      serialize_tracking_message(msg, c, m->data);
    }
  }
}

void setup()
{
    cell_colour[0] = (configuration[0]);
    cell_colour[1] = (configuration[0]);
    cell_colour[2] = (configuration[0]);
    cell_colour[3] = (configuration[0]);

    cell_msg[0] = (configuration[1]);
    cell_msg[1] = (configuration[1]);
    cell_msg[2] = (configuration[1]);
    cell_msg[3] = (configuration[1]);
}

void loop()
{
	for(int i = 0; i < 4; ++i)
	{
	
		switch(cell_colour[i])
		{
		 case 0:
		 if (i==0){
		 	set_LED_with_brightness(cell_id[i], GREEN, HIGH);
		 } else {
		 	set_LED_with_brightness(cell_id[i], BLUE, HIGH);
		 }
		 set_IR_message(NULL, i);
		 break;
		 case 2:
		 set_all_LEDs_hex( colour2 );
		 set_IR_message(NULL, i);
		 break;
		 case 1:
		 set_LED_with_brightness(cell_id[i], RED, MEDIUM);
		 int broadcast = 1;
		 if (broadcast){
		 	IR_message_tx[i].type = 1; 
		 	IR_message_tx[i].data[0] = cell_msg[i];
		 	set_IR_message(&IR_message_tx[i], i);
		 } else {
		 	set_IR_message(NULL, i);
		 }
		 break;
		 default:
		 break;
		}
	}
}


int main() {
    module_init();
    
    module_IR_message_rx = IR_rx;
    
    module_start(setup, loop);
	
    return 0;
}
