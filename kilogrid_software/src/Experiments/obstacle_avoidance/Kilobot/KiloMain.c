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
	RANDOM_WALK = 0x01
} kilobot_state_t;

kilobot_state_t state = INIT;

tracking_user_data_t tracking_data;

double check_period = 5;
double turn_mu = 3;
double turn_sigma = 0.5;
double fw_mu = 10;
double fw_sigma = 1;
double stuck_timeout = 20;

void on_escape_wall(){}

void message_rx(IR_message_t *m, distance_measurement_t *d)
{
    if(m->type == MESSAGE_GRADIENT)
    {
      if(state == INIT)
      {
	kilob_gradient_tracking_config_add(0, kilob_gradient_tracking_decreasing, on_escape_wall, 1);
	//kilob_gradient_tracking_config_add(1, kilob_gradient_tracking_decreasing, on_escape_wall, 1);
	set_color(RGB(0,1,0));
	state = RANDOM_WALK;
      }
      kilob_gradient_tracking_message_received(m, d);
    }
}



void setup() {
    // put your setup code here, will be run once at the beginning
	g_ran_set_seed_uint32(0);
	kilob_tracking_init();
	tracking_data.byte[0] = kilo_uid;

	kilob_gradient_tracking_init(check_period, turn_mu,
		turn_sigma, turn_mu, turn_sigma,
		stuck_timeout);
	kilob_random_walk_init(fw_mu, fw_sigma, turn_mu, turn_sigma);
}

void loop()
{
  switch(state)
  {   
	case RANDOM_WALK:
	if(kilob_gradient_has_active())
	{
	 kilob_gradient_tracking();
	}
	else
	{
      	 kilob_random_walk();
	}
	break;
	
	default:
	break;
  }
	tracking_data.byte[0] = kilo_uid;
	kilob_tracking(&tracking_data);
}  

int main() {
    kilo_init();
	utils_init();

	utils_message_rx = message_rx; // register IR reception callback
	
    kilo_start(setup, loop);

    return 0;
}
