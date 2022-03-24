#include <avr/io.h>
#include <avr/wdt.h> 
#include <util/delay.h>     // delay macros
#include <avr/interrupt.h>

#include "module.h"
#include "moduleLED.h"
#include "moduleIR.h"
#include "CAN.h"
#include "math.h"
#include "../communication.h"

#define GRADIENT_MASK 63
#define TYPE_SHIFT 6
#define GRADIENT_SHIFT 4
#define SITE_MASK 15

volatile uint8_t nest_gradient[4] = {0, 0, 0, 0};
volatile uint8_t wall_gradient[4] = {0, 0, 0, 0};
volatile uint8_t site_gradient[4] = {0, 0, 0, 0};
volatile uint8_t site_walk_gradient[4] = {0, 0, 0, 0}; //works as module id for nest
volatile uint8_t pheromone[4] = {0, 0, 0, 0};
volatile uint8_t type[4] = {0, 0, 0, 0}; // nothing: 00, nest: 01, siteA: 10 siteB: 11
uint32_t pheromone_timer[4];
uint32_t nest_pheromone_timer[4][2];
uint32_t pheromone_evaporation_timer = SEC_TO_TICKS(60);
uint8_t blacklist[4][100];  //x 0 y  drop    x 1 y reinforce
uint8_t blacklist_index[4] = {0,0,0,0};
uint32_t blacklist_timer;
uint32_t blacklist_reset_period = SEC_TO_TICKS(60);
uint8_t pheromone_initialized = 0;

uint8_t nest_pheromone[4][2]; // [c][0] siteA [c][1] siteB

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
  if(!CRC_error && m->type == MESSAGE_PHEROMONE_DROP){
    for(int i = 0; i < blacklist_index[c]; ++i)
    {
      if(m->data[0] == blacklist[c][i])
      {
        return;
      }
    }
    blacklist[c][blacklist_index[c]] = m->data[0];
    blacklist_index[c]++;
    if(type[c] != 1)
    {
      if(pheromone[c] == 0)
      {
        pheromone_timer[c] = module_ticks + pheromone_evaporation_timer;
      }
      pheromone[c]++;
      CAN_message_t* msg = next_CAN_message();
      if(msg != NULL){
  tracking_data.byte[0] = 1;
        tracking_data.byte[1] = pheromone[c];
        tracking_data.byte[2] = 0;
  serialize_tracking_message(msg, cell_id[c], &tracking_data);
    }
    }
    if(type[c] == 1)
    {
  if(nest_pheromone[c][m->data[1]] == 0)
  {
  nest_pheromone_timer[c][m->data[1]] = module_ticks + pheromone_evaporation_timer;
  }
  nest_pheromone[c][m->data[1]]++;
      CAN_message_t* msg = next_CAN_message();
      if(msg != NULL){
  tracking_data.byte[0] = 1;
        tracking_data.byte[1] = nest_pheromone[c][0] + nest_pheromone[c][1];
  tracking_data.byte[2] = nest_pheromone[c][0];
  tracking_data.byte[3] = nest_pheromone[c][1];
  serialize_tracking_message(msg, cell_id[c], &tracking_data);
    }
    } 
}
}

void setup(){
  if(configuration[0] == 0)
  {
    nest_gradient[0] = (configuration[1] & GRADIENT_MASK);
    nest_gradient[1] = (configuration[2] & GRADIENT_MASK);
    nest_gradient[2] = (configuration[3] & GRADIENT_MASK);
    nest_gradient[3] = (configuration[4] & GRADIENT_MASK);
  
    type[0] = (configuration[1] >> TYPE_SHIFT);
    type[1] = (configuration[2] >> TYPE_SHIFT);
    type[2] = (configuration[3] >> TYPE_SHIFT);
    type[3] = (configuration[4] >> TYPE_SHIFT);
  }
  if(configuration[0] == 1)
  {    
    site_gradient[0] = (configuration[1] & SITE_MASK);
    site_gradient[1] = (configuration[2] & SITE_MASK);
    site_gradient[2] = (configuration[3] & SITE_MASK);
    site_gradient[3] = (configuration[4] & SITE_MASK);

    wall_gradient[0] = (configuration[1] >> GRADIENT_SHIFT);
    wall_gradient[1] = (configuration[2] >> GRADIENT_SHIFT);
    wall_gradient[2] = (configuration[3] >> GRADIENT_SHIFT);
    wall_gradient[3] = (configuration[4] >> GRADIENT_SHIFT);
  }
  if(configuration[0] == 2)
  {
    site_walk_gradient[0] = configuration[1];
    site_walk_gradient[1] = configuration[2];
    site_walk_gradient[2] = configuration[3];
    site_walk_gradient[3] = configuration[4];
  }

  for(int i = 0; i < 4; ++i)
  {
    blacklist_timer = module_ticks + blacklist_reset_period;
    pheromone_timer[i] = module_ticks;
    nest_pheromone_timer[i][0] = module_ticks;
    nest_pheromone_timer[i][1] = module_ticks;
    for(int j = 0; j < 50; ++j)
    {
      blacklist[i][j] = 0;
    }
  }
  pheromone_initialized = 0;
}

void loop()
{
  for(int i = 0; i < 4; ++i)
    { 
      switch(type[i])
      {
      case 0:
      if(pheromone[i] == 0)
      {
        set_LED_with_brightness(cell_id[i], LED_OFF, LOW);
      }
      if(pheromone[i] > 0 && pheromone[i] < 6)
      {
        set_LED_with_brightness(cell_id[i], YELLOW, 1);
      }
      if(pheromone[i] >= 6 && pheromone[i] < 12)
      {
        set_LED_with_brightness(cell_id[i], YELLOW, 10);
      }
      if(pheromone[i] >= 12)
      {
        set_LED_with_brightness(cell_id[i], YELLOW, 40);
      }
      break;
    
      case 1:
        set_LED_with_brightness(cell_id[i], GREEN, MEDIUM);
        break;
      case 2:
        set_LED_with_brightness(cell_id[i], RED, MEDIUM);
        break;
      case 3:
        set_LED_with_brightness(cell_id[i], BLUE, MEDIUM);
        break;
            
      default:
        set_LED_with_brightness(cell_id[i], LED_OFF, LOW);
      }

      IR_message_tx[i].type = MESSAGE_GRADIENT; 
      IR_message_tx[i].data[0] = type[i];
      IR_message_tx[i].data[1] = nest_gradient[i];
      IR_message_tx[i].data[2] = site_gradient[i];
      IR_message_tx[i].data[3] = wall_gradient[i];
      IR_message_tx[i].data[7] = site_walk_gradient[i];
      if(type[i] != 1)
      {
      IR_message_tx[i].data[4] = nest_gradient[i] + ceil(pheromone[i]/6); 
      IR_message_tx[i].data[5] = pheromone[i]; 
      }
      if(type[i] == 1)
      { 
      IR_message_tx[i].data[4] = nest_gradient[i];// + ceil((nest_pheromone[i][0] + nest_pheromone[i][1])/3);
      IR_message_tx[i].data[5] = nest_pheromone[i][0];
      IR_message_tx[i].data[6] = nest_pheromone[i][1];
      }
      set_IR_message(&IR_message_tx[i], i);      
      if(module_ticks > pheromone_timer[i] && type[i] != 1)
      {
        if(pheromone[i] > 0)
        {
          pheromone[i]--;
      CAN_message_t* msg = next_CAN_message();
      if(msg != NULL){
  tracking_data.byte[0] = 1;
        tracking_data.byte[1] = pheromone[i];
  serialize_tracking_message(msg, cell_id[i], &tracking_data);
    }
          if(pheromone[i] > 0)
          {
              pheromone_timer[i] = module_ticks + pheromone_evaporation_timer;
          }
        }
      }
  if(module_ticks > nest_pheromone_timer[i][0] && type[i] == 1)
  {
    if(nest_pheromone[i][0] > 1) 
    {
      nest_pheromone_timer[i][0] = module_ticks + pheromone_evaporation_timer;
    }
    if(nest_pheromone[i][0] > 0)
    {
      nest_pheromone[i][0]--;       
      CAN_message_t* msg = next_CAN_message();
      if(msg != NULL){
  tracking_data.byte[0] = 1;
        tracking_data.byte[1] = nest_pheromone[i][0] + nest_pheromone[i][1];
  tracking_data.byte[2] = nest_pheromone[i][0];
  tracking_data.byte[3] = nest_pheromone[i][1];
  serialize_tracking_message(msg, cell_id[i], &tracking_data);
    }
    }
  }
  if(module_ticks > nest_pheromone_timer[i][1] && type[i] == 1)
  {
    if(nest_pheromone[i][1] > 1) 
    {
      nest_pheromone_timer[i][1] = module_ticks + pheromone_evaporation_timer;
    }
    if(nest_pheromone[i][1] > 0)
    {
      nest_pheromone[i][1]--;       
      CAN_message_t* msg = next_CAN_message();
      if(msg != NULL){
  tracking_data.byte[0] = 1;
        tracking_data.byte[1] = nest_pheromone[i][0] + nest_pheromone[i][1];
  tracking_data.byte[2] = nest_pheromone[i][0];
  tracking_data.byte[3] = nest_pheromone[i][1];
  serialize_tracking_message(msg, cell_id[i], &tracking_data);
    }
    }
  }
    }
    if(module_ticks > blacklist_timer)
      {
        for(int j = 0; j < 4; ++j)
        {
          blacklist_index[j] = 0;
          blacklist_index[j] = 0;
        }
        blacklist_timer += blacklist_reset_period;
      }
     if(!pheromone_initialized)
      {
       for(int j = 0; j < 4; ++j)
       {
        pheromone[j] = 0;
       }
       pheromone_initialized = 1; 
      }
}


int main() {
    module_init();
    
    module_IR_message_rx = IR_rx;
    
    module_start(setup, loop);
  
    return 0;
}
