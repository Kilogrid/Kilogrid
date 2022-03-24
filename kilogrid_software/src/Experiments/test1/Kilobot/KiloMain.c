#include <stdlib.h>

#include "kilolib.h"
#include "utils.h"
#include <math.h>
#include "kilob_tracking.h"
#include "kilo_rand_lib.h"
//#include "../communication.h"
//#include "kilob_messaging.h"
//#include "kilob_random_walk.h"

typedef enum{
	INIT = 0x00,
	RANDOM_WALK = 0x01
} kilobot_state_t;

kilobot_state_t state = INIT;
uint8_t msg_received = 0;

tracking_user_data_t tracking_data;
uint32_t counter = 0;

void message_rx(IR_message_t *m, distance_measurement_t *d)
{
    if(m->type == 1)
    {      
		msg_received = 1;
		counter += 1;
    }
}

void setup() {
    // put your setup code here, will be run once at the beginning
	g_ran_set_seed_uint32(0);
	kilob_tracking_init();
	tracking_data.byte[0] = kilo_uid;

}

void loop() {
	if (msg_received){
		msg_received=0;
		set_color(RGB(0,3,0));
		delay(100);
	} else {
		set_color(RGB(0,0,0));
	}
	tracking_data.byte[1] = counter;
	tracking_data.byte[2] = (counter >> 8);
	tracking_data.byte[3] = (counter >> 16);
	
	kilob_tracking(&tracking_data);
}  

int main() {
    kilo_init();
	utils_init();

	utils_message_rx = message_rx; // register IR reception callback
	
    kilo_start(setup, loop);

    return 0;
}
