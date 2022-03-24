#include <avr/io.h>
#include <avr/wdt.h> 
#include <util/delay.h>     // delay macros
#include <avr/interrupt.h>

#include "module.h"
#include "moduleLED.h"
#include "moduleIR.h"
#include "CAN.h"

#include "../communication.h"


volatile uint8_t wall_gradient[4] = {0, 0, 0, 0};
volatile uint8_t wet[4] = {0, 0, 0, 0}; //  0->dry    1->wet     
uint32_t evaporation_period = SEC_TO_TICKS(120);
uint32_t evaporation_timer[4] = {0, 0, 0, 0};
uint8_t robot_uid[4] = {0, 0, 0, 0};

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
  if(!CRC_error && m->type == MESSAGE_WATER_DROP){
	for(int i = 0; i < 4; ++i)
	{
	 if(!wet[i] && m->data[1] == module_uid_x_coord && m->data[2] == module_uid_y_coord && m->data[3] == cell_id[i])
	 {
	  robot_uid[i] = m->data[0];
	  wet[i] = 1;
	  evaporation_timer[i] = module_ticks + evaporation_period;
    CAN_message_t* msg = next_CAN_message();
    if(msg != NULL){
      tracking_data.byte[0] = 1;
      tracking_data.byte[1] = wet[i];
      tracking_data.byte[2] = robot_uid[i];
      serialize_tracking_message(msg, cell_id[i], &tracking_data);
    }
	 }
	}    
	}
}


void setup()
{
 for(int i = 0; i < 4; ++i)
 {
  wall_gradient[i] = (configuration[i]);
}
}

void loop()
{
	for(int i = 0; i < 4; ++i)
  	{ 
    	switch(wet[i])
    	{
	     case 0:
	     set_LED_with_brightness(cell_id[i], YELLOW, LOW);
	     break;	
    	 case 1:
      set_LED_with_brightness(cell_id[i], GREEN, MEDIUM);
      if(module_ticks > evaporation_timer[i])
      {
	       wet[i] = 0;
         CAN_message_t* msg = next_CAN_message();
    if(msg != NULL){
      tracking_data.byte[0] = 1;
      tracking_data.byte[1] = wet[i];
      tracking_data.byte[2] = robot_uid[i];
      serialize_tracking_message(msg, cell_id[i], &tracking_data);
    }
      }
      break;
            
      default:
      break;
}

      IR_message_tx[i].type = MESSAGE_GRADIENT; 
      IR_message_tx[i].data[0] = wet[i];
      IR_message_tx[i].data[1] = module_uid_x_coord;
      IR_message_tx[i].data[2] = module_uid_y_coord;
      IR_message_tx[i].data[3] = cell_id[i];
      IR_message_tx[i].data[4] = robot_uid[i]; 
      IR_message_tx[i].data[5] = wall_gradient[i];
      
      set_IR_message(&IR_message_tx[i], i);
}
}

int main() {
    module_init();
    
    module_IR_message_rx = IR_rx;
    
    module_start(setup, loop);
	
    return 0;
}
