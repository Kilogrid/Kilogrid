#include <avr/io.h>
#include <avr/wdt.h> 
#include <util/delay.h>     // delay macros
#include <avr/interrupt.h>

#include "module.h"
#include "moduleLED.h"
#include "moduleIR.h"
#include "CAN.h"

#include "../communication.h"

#define GRADIENT_SHIFT 4
#define SITE_MASK 15

volatile uint8_t wall_gradient[4] = {0, 0, 0, 0};
volatile uint8_t obstacles_gradient[4] = {0, 0, 0, 0};
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
    obstacles_gradient[0] = (configuration[0] & SITE_MASK);
    obstacles_gradient[1] = (configuration[1] & SITE_MASK);
    obstacles_gradient[2] = (configuration[2] & SITE_MASK);
    obstacles_gradient[3] = (configuration[3] & SITE_MASK);

    wall_gradient[0] = (configuration[0] >> GRADIENT_SHIFT);
    wall_gradient[1] = (configuration[1] >> GRADIENT_SHIFT);
    wall_gradient[2] = (configuration[2] >> GRADIENT_SHIFT);
    wall_gradient[3] = (configuration[3] >> GRADIENT_SHIFT);
}



void loop()
{
	for(int i = 0; i < 4; ++i)
	{
	 IR_message_tx[i].type = MESSAGE_GRADIENT; 
	 IR_message_tx[i].data[0] = wall_gradient[i];
	 IR_message_tx[i].data[1] = obstacles_gradient[i];
	 set_IR_message(&IR_message_tx[i], i);
	
	switch(obstacles_gradient[i])
	{
	 case 3:
	 set_LED_with_brightness(cell_id[i], RED, HIGH);
	 break;
	 case 2:
	 set_LED_with_brightness(cell_id[i], RED, MEDIUM);
	 break;
	 case 1:
	 set_LED_with_brightness(cell_id[i], RED, LOW);
	 break;
	 default:
	 break;
	}

	switch(wall_gradient[i])
	{
	 case 2:
	 set_LED_with_brightness(cell_id[i], RED, MEDIUM);
	 break;
	 case 1:
	 set_LED_with_brightness(cell_id[i], RED, LOW);
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
