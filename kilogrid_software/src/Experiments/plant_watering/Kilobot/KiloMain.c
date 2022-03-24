#include <stdlib.h>

#include "kilolib.h"
#include "utils.h"
#include <math.h>
#include "kilob_wall_avoidance.h"
#include "kilob_tracking.h"
#include "kilo_rand_lib.h"
#include "../communication.h"
#include "kilob_gradient.h"
#include "kilob_messaging.h"
#include "kilob_random_walk.h"



typedef enum{
	INIT = 0x00,
	RANDOM_WALK = 0x01,
	GARDENING = 0x02,
	END = 0x03
} kilobot_state_t;

kilobot_state_t state = INIT;
tracking_user_data_t tracking_data;

double check_period = 3;
double small_turn_mu = 2;
double large_turn_mu = 4;
double small_turn_sigma = 0.5;
double large_turn_sigma = 1;
double fw_mu = 15;
double fw_sigma = 1;
double stuck_timeout = 20;

uint32_t gardening_period = MS_TO_TICKS(5000);
uint32_t gardening_timer;

uint32_t random_walk_timer;


uint8_t cell_x;
uint8_t cell_y;
uint8_t sc;

uint8_t water_supply = 100;
uint8_t water_dropped = 0;

IR_message_t* msg;


void drop_water()
{
  if((msg = kilob_message_send()) != NULL) {
          msg->type = MESSAGE_WATER_DROP;
          msg->data[0] = kilo_uid;
	  msg->data[1] = cell_x;
	  msg->data[2] = cell_y;
	  msg->data[3] = sc;
        }
}

// turn flag on message reception

void on_escape_wall(){}

void message_rx(IR_message_t *m, distance_measurement_t *d)
{
	if(state == END) return;
    if(m->type == MESSAGE_GRADIENT)
    {
	if(state == INIT)
	{
      	 kilob_gradient_tracking_config_add(5, kilob_gradient_tracking_decreasing, on_escape_wall, 1);
      	 random_walk_timer = kilo_ticks + MS_TO_TICKS(g_ran_uniform()*15000);
	 //set_color(RGB(0,0,1));
	 state = RANDOM_WALK;
	}
	if(state == RANDOM_WALK && m->data[0] == 0 && kilo_ticks > random_walk_timer)
	{
	 //stop();
	 cell_x = m->data[1];
	 cell_y = m->data[2];
	 sc = m->data[3];
	 //set_color(RGB(0,1,0));
	 state = GARDENING;
	 gardening_timer = kilo_ticks + gardening_period;
	} 
	if(state == GARDENING && !water_dropped && m->data[0] == 1 && m->data[1] == cell_x && m->data[2] == cell_y && m->data[3] == sc)
	{
		if(m->data[4] == kilo_uid) {
		if(water_supply > 0)
		{
			--water_supply;
	 		water_dropped = 1;
		}
		if(water_supply == 0)
		{
			stop();
			state = END;
		}

	} else {
		move_straight();
		state = RANDOM_WALK;
	} 
	 
	}
		if(state == RANDOM_WALK)
		{
			kilob_gradient_tracking_message_received(m, d);
	  	}
	  
    }
}





void setup() {
    // put your setup code here, will be run once at the beginning


	g_ran_set_seed_uint32(0);

	kilob_tracking_init();
	tracking_data.byte[0] = kilo_uid;
	tracking_data.byte[1] = state;

	kilob_gradient_tracking_init(check_period, small_turn_mu,
		small_turn_sigma, large_turn_mu, large_turn_sigma,
		stuck_timeout);
	kilob_random_walk_init(RW_FORWARD_MU, RW_FORWARD_SIGMA,
	RW_ROTATION_CONST, RW_ROTATION_SPAN);
  
}

void loop()
{
  switch(state)
  {   
    case RANDOM_WALK:
      set_color(RGB(0,0,1));
      if(kilob_gradient_has_active())
      {
	kilob_gradient_tracking();
      }
      else
      {
	kilob_random_walk();
      }
   break;
     
     case GARDENING:
     stop();
     set_color(RGB(0,1,0));
     drop_water();
     if(kilo_ticks > gardening_timer)
     {
      water_dropped = 0;
      //set_color(RGB(0,0,1));
      move_straight();
      state = RANDOM_WALK;
     }
    break;

    case END:
    set_color(RGB(0,0,0));
    break;
   
   default:
    break;
  }
	tracking_data.byte[0] = kilo_uid;
  	tracking_data.byte[1] = state;
	tracking_data.byte[2] = water_supply;
	kilob_tracking(&tracking_data);
  
}

int main() {
    kilo_init();
	utils_init();

	utils_message_rx = message_rx; // register IR reception callback
	
    kilo_start(setup, loop);

    return 0;
}
