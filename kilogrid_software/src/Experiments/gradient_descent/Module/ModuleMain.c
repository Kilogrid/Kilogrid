#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>     // delay macros
#include <avr/interrupt.h>

#include "module.h"
#include "moduleLED.h"
#include "moduleIR.h"
#include "CAN.h"

#include "../communication.h"

volatile uint8_t cell_x[4] = {0, 0, 0, 0};
volatile uint8_t cell_y[4] = {0, 0, 0, 0};
volatile uint8_t cell_role[4] = {0, 0, 0, 0};
volatile uint8_t cell_msg[4] = {0, 0, 0, 0};

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
  cell_x[0] = (configuration[0]);
  cell_x[1] = (configuration[0]);
  cell_x[2] = (configuration[0]);
  cell_x[3] = (configuration[0]);

  cell_y[0] = (configuration[1]);
  cell_y[1] = (configuration[1]);
  cell_y[2] = (configuration[1]);
  cell_y[3] = (configuration[1]);

  cell_role[0] = (configuration[2]);
  cell_role[1] = (configuration[2]);
  cell_role[2] = (configuration[2]);
  cell_role[3] = (configuration[2]);
}

void loop()
{
	for(int i = 0; i < 4; ++i)
	{
     IR_message_tx[i].type = 1;
     IR_message_tx[i].data[0] = cell_x[i];
     IR_message_tx[i].data[1] = cell_y[i];
     IR_message_tx[i].data[2] = cell_role[i];

     set_IR_message(&IR_message_tx[i], i);

		switch(cell_role[i])
		{
		 case 0:
		   set_LED_with_brightness(cell_id[i], BLUE, HIGH);
		 break;
		 case 10:
		   set_LED_with_brightness(cell_id[i], GREEN, HIGH);
		 break;
     case 42:
       set_LED_with_brightness(cell_id[i], RED, HIGH);
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
