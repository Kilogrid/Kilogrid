// macro if we are in sim or reality -> command out if on real robot
//#define DYNAMICRANGE
//#define OPTIMALSAMPLE
// #define TRACKQUALITY


// imports
#include <stdlib.h>
#include "kilolib.h"
#include <math.h>
#include "utils.h"
#include <util/delay.h>     // delay macros
#include "kilob_tracking.h"
#include "kilo_rand_lib.h"
#include "../communication.h"
#include "kilob_messaging.h"


/*-----------------------------------------------------------------------------------------------*/
/* Define section here you can define values, e.g., messages types                               */
/*-----------------------------------------------------------------------------------------------*/
#define PI 3.14159265358979323846
// options
#define UNCOMMITTED 0
#define UNINITIALISED 20

// counters
#define AVOIDANCE_STRAIGHT_COUNTER_MAX 300
#define STRAIGHT_COUNTER_MAX 300
#define TURN_COUNTER_MAX 300
#define MAX_WAYPOINT_TIME 36000
#define ROTATION_SPEED 38

// parameters
#define SAMPLE_COUNTER_MAX 60
#define SAMPLE_TICKS 32
#define NUMBER_OF_OPTIONS 2
#define INITIAL_ROBOT_COMMITMENT 1

#define UPDATE_TICKS 60
#define BROADCAST_TICKS 15

#define MIN_COMMUNICATION_RANGE 1  // is used in dynamic com update
#define COMMUNICATION_THRESHOLD_TIMER (1875*5) //1875 // in ticks - should be 1 min????
#define PARAM 0.0

// flags
#define CALIBRATED true


/*-----------------------------------------------------------------------------------------------*/
/* Enum section - here we can define useful enums                                                */
/*-----------------------------------------------------------------------------------------------*/
typedef enum{
    false = 0,
    true = 1,
} bool;

typedef enum{
    STOP,
    MOVE_STRAIGHT,
    MOVE_LEFT,
    MOVE_RIGHT,
} state;


// variables for testing
state last_state;
state current_state;

tracking_user_data_t tracking_data;

/*-----------------------------------------------------------------------------------------------*/
/* Function for setting the motor speed.                                                         */
/*-----------------------------------------------------------------------------------------------*/
void set_motion(){
    // only change if motion type changed
    if(last_state != current_state){
        last_state = current_state;

        spinup_motors();
        switch(current_state) {
            case MOVE_STRAIGHT:
                set_motors(kilo_straight_left, kilo_straight_right);
                break;
            case MOVE_LEFT:
                set_motors(kilo_turn_left, 0);
                break;
            case MOVE_RIGHT:
                set_motors(0, kilo_turn_right);
                break;
            case STOP:
            default:
                set_motors(0, 0);
        }
    }
}









/*-----------------------------------------------------------------------------------------------*/
/* This function implements the callback, for when the robot receives an infrared message (here  */
/* only from the kilogrid)                                                                       */
/*-----------------------------------------------------------------------------------------------*/
void message_rx( IR_message_t *msg, distance_measurement_t *d ) {
    

    // check for messages - receiving turn/straight messages from the kilogui 
    if (msg->type == 125) {
		current_state = MOVE_STRAIGHT;
		set_color(RGB(0,3,0));
    } else if (msg->type == 126) { 
   		current_state = MOVE_LEFT;
   		set_color(RGB(3,0,0));
    }else if (msg->type == 127){
		current_state = MOVE_RIGHT;
		set_color(RGB(0,0,3));
    }



// tracking_data.byte[1] = msg->type;
// 	    tracking_data.byte[2] = msg->data[0];
// 	    tracking_data.byte[3] = msg->data[1];
// 	    tracking_data.byte[4] = msg->data[2];
// 	    tracking_data.byte[5] = msg->data[3];
// 	    kilob_tracking(&tracking_data);
// 	    set_color(RGB(0,0,3));

    // if(msg->type == 67 && init_flag){
    //     received_option_msg = msg->data[0];
    //     received_virtual_agent_msg_flag = true;
    // }else if(msg->type == 11){
    //     // this message type is send by the kilogrid
    //     received_x = msg->data[0];
    //     received_y = msg->data[1];
    //     received_ground = msg->data[2];
    //     received_role = msg->data[3];
    //     received_grid_msg_flag = true;
    //     init_flag = true;
    // }
}


/*-----------------------------------------------------------------------------------------------*/
/* Callback function for successful transmission                                                 */
/*-----------------------------------------------------------------------------------------------*/
void tx_message_success() {
    
    return;
}


/*-----------------------------------------------------------------------------------------------*/
/* This function implements the sending to the kilogrid. you should call this function every     */
/* loop cycle because in reality you dont have a indicator if the message was received so we     */
/* have to send it multiple times. The when and how often to send a message should be            */
/* implemented here!                                                                             */
/*-----------------------------------------------------------------------------------------------*/
void message_tx(){
    // implementation differs because in simulation we use the debugstruct - faster and easier to
    // understand
    // in reality we send infrared msg - we send more than one to make sure that the messages arrive!

    return;
    
}





/*-----------------------------------------------------------------------------------------------*/
/* Init function                                                                                 */
/*-----------------------------------------------------------------------------------------------*/
void setup(){
    // for tracking the robot in real life
    kilob_tracking_init();
    kilob_messaging_init();
    tracking_data.byte[0] = kilo_uid;
    
    // Initialise random seed
    uint8_t seed = rand_hard();
    rand_seed(seed);
    seed = rand_hard();
    srand(seed);

    // Initialise motors
    set_motors(0,0);

    // Initialise time to 0
    kilo_ticks = 0;
    last_state = STOP;
    current_state = STOP;
}


/*-----------------------------------------------------------------------------------------------*/
/* Main loop                                                                                     */
/*-----------------------------------------------------------------------------------------------*/
void loop() {
	set_motion();
	
    kilob_tracking(&tracking_data);

    _delay_ms(30);  // needed because otherwise one cycle would not take 30 ms -> counter would not work
}


/*-----------------------------------------------------------------------------------------------*/
/* Main function - obviously needs to be implemented by both platforms.                          */
/*-----------------------------------------------------------------------------------------------*/
int main(){
    // initialize the hardware of the robot
    kilo_init();
    // now initialize specific things only needed for one platform
    // initalize utils
    utils_init();
    // callback for received messages
    kilo_message_rx = message_rx;
    kilo_message_tx_success = tx_message_success;
    // start control loop
    kilo_start(setup, loop);
    return 0;
}