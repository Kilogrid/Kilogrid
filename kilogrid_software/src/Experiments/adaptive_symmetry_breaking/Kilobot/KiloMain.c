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
#define STRAIGHT_COUNTER_MAX 150
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
    AVOIDANCE_TURN_LEFT,
    AVOIDANCE_TURN_RIGHT,
    AVOIDANCE_STRAIGHT
} state;


// debug tmp
uint8_t debug_counter = 0;

/*-----------------------------------------------------------------------------------------------*/
/* Robot state variables.                                                                        */
/*-----------------------------------------------------------------------------------------------*/
uint8_t robot_gps_x;  // current x position
uint8_t robot_gps_y;  // current y position

uint8_t robot_gps_x_last;
uint8_t robot_gps_y_last;

uint8_t goal_gps_x = 0;
uint8_t goal_gps_y = 0;

int32_t robot_orientation;
bool calculated_orientation = false;

uint8_t robot_commitment = UNINITIALISED;
float robot_commitment_quality = 0.0;
float robot_commitment_quality_tmp = 0.0;

uint8_t communication_range = 0;  // communication range in cells
uint8_t max_communication_range = 0;  // for dynamic setting

uint8_t received_option = 0;

bool robot_hit_wall = false;
bool robot_hit_semi_wall = false;

uint32_t last_waypoint_time = 0;
uint32_t state_counter = 0;
state current_state = MOVE_STRAIGHT;
state last_state = STOP;

// sample variables
uint32_t last_sample_ticks = 0;
uint32_t sample_counter_max_noise = 0;

uint32_t sample_counter = 0;  // counts how often we sampled
uint32_t sample_op_counter = 0;  // counter on how often we encounter our op tp sample
uint8_t op_to_sample = 1;  // option we want to sample -> start with the crappy one ?

bool discovered = false;
uint8_t discovered_option = 0;
double discovered_quality = 0.0;

// commitment update variables
uint32_t last_update_ticks = 0;
uint32_t update_ticks_noise = 0;

uint32_t last_orientation_update = 0;

bool new_robot_msg = false;

tracking_user_data_t tracking_data;

/*-----------------------------------------------------------------------------------------------*/
/* Communication variables - used for communication and stuff                                    */
/*-----------------------------------------------------------------------------------------------*/
// how often we try to send the msg - in simulation once is sufficient
#define MSG_SEND_TRIES 10
// Kilobot -> Kilogrid
uint32_t msg_counter_sent = MSG_SEND_TRIES + 1;  // counts the messages sent
uint32_t msg_number_send = 0;  // change if you want to send a msg
uint32_t msg_number_current_send = 0;  // var for checking against the last
// Kilogrid -> Kilobot
bool init_flag = false;
bool received_grid_msg_flag = false;
bool received_virtual_agent_msg_flag = false;
// message content
IR_message_t* message;
uint32_t msg_counter = 0;

// tmp variables for saving in callback function
uint8_t received_option_msg = 0;
//int32_t received_kilo_uid = 0;

uint8_t received_x = 0;
uint8_t received_y = 0;
uint8_t received_ground = 0;
uint8_t received_role = 0;

// broadcast variables
uint32_t last_broadcast_ticks = 0;


/*-----------------------------------------------------------------------------------------------*/
/* Arena variables                                                                               */
/*-----------------------------------------------------------------------------------------------*/
uint8_t current_ground = 0;
const uint8_t CELLS_X = 20;
const uint8_t CELLS_Y = 40;


uint32_t normalize_angle(double angle){
    while(angle>180){
        angle=angle-360;
    }
    while(angle<-180){
        angle=angle+360;
    }

    return (int32_t) angle;
}


/*-----------------------------------------------------------------------------------------------*/
/* Retruns a random number in range_rnd.                                                         */
/*-----------------------------------------------------------------------------------------------*/
unsigned int GetRandomNumber(unsigned int range_rnd){
    int randomInt = RAND_MAX;
    while (randomInt > 30000){
        randomInt = rand();
    }
    unsigned int random = randomInt % range_rnd + 1;
    return random;
}


/*-----------------------------------------------------------------------------------------------*/
/* Sample function for the ground - sampling should take 30 sec                                  */
/*-----------------------------------------------------------------------------------------------*/
void sample(){
    // sample every second
    if(kilo_ticks > last_sample_ticks + SAMPLE_TICKS) {
        last_sample_ticks = kilo_ticks;
        // if we are currently hitting a wall we do not want to measure the options!
        if(robot_hit_wall) return;

        // check if we reached our sampling time
        if(sample_counter < sample_counter_max_noise){
            sample_counter++;
            if (current_ground == op_to_sample){
                sample_op_counter++;
            }
            robot_commitment_quality_tmp = (float)sample_op_counter/(float)sample_counter_max_noise;
        }else{ // sampling finished
            // update discovered option
            discovered_option = op_to_sample;
            discovered_quality = (float)sample_op_counter/(float)sample_counter;
            // set my quality to the measured quality if it's the robot commitment
            // also delete the last commitment, bc the robot can only store one!
            if (op_to_sample == robot_commitment){
                robot_commitment_quality = discovered_quality;
            }else{
                // set discovery flag if we discovered something new!
                discovered = true;
            }

            // reset sampling
            op_to_sample = current_ground;
            sample_counter = 0;
            sample_op_counter = 0;
            sample_counter_max_noise = SAMPLE_COUNTER_MAX;
        }
    }
}

/*-----------------------------------------------------------------------------------------------*/
/* Function for updating the commitment state (wrt to the received message)                      */
/*-----------------------------------------------------------------------------------------------*/
void update_commitment() {
    if(kilo_ticks > last_update_ticks + update_ticks_noise){
        // set next update
        last_update_ticks = kilo_ticks;
        // noise +- 5%
        update_ticks_noise = UPDATE_TICKS; //+ ((GetRandomNumber(10000) % ((uint8_t)(UPDATE_TICKS/10) + 1)) - (uint8_t)(UPDATE_TICKS/20));;


        double quality;
        bool social = false;
        bool individual = false;

        // if robot made a discovery we have a discovery quality
        if(discovered){
            quality = discovered_quality;
        }else{  // robot did not sample enough yet
            quality = 0.0;
        }

        // Discovery and COMPARE: found a better option (in case of discovery robot is uncommitted
        // thus robot_commitment_quality should be 0
        if(quality > robot_commitment_quality + PARAM){
            individual = true;
        }
        // RECRUITMENT and DIRECT-SWITCH: message with different option
        if(new_robot_msg && robot_commitment != received_option && received_option != UNCOMMITTED){  // && !(robot_commitment_quality <= 0.001 && robot_commitment != UNCOMMITTED)
            social = true;
        }

        // if both are true do a flip
        if(individual && social) {
            if (GetRandomNumber(10000) % 2) {
                individual = true;
                social = false;
            } else {
                individual = false;
                social = true;
            }
        }

        if(individual){
            /// set new commitment
            robot_commitment = discovered_option;
            robot_commitment_quality = discovered_quality;
        /// CROSS-INHIBITION SOCIAL PATTERN
        }else if(social){
            // basically the robot is recruited when it finishes sampling
            if (robot_commitment == UNCOMMITTED){
                // set new commitment
                robot_commitment = received_option;
                // set new option the robot should sample
                op_to_sample = received_option;
                robot_commitment_quality = 0.0;
            } else {  /// Robot is committed
                /// CROSS-INHIBITION - becomes uncommitted
                robot_commitment = UNCOMMITTED;
                robot_commitment_quality = 0.0;
                op_to_sample = current_ground;
            }
            /// reset sampling to make a new estimate on current commitment
            sample_op_counter = 0;
            sample_counter = 0;
        }
        // reset discovery and new message, so that they are not used again
        new_robot_msg = false;
        discovered = false;
    }
}


void update_communication_range(){
}


/*-----------------------------------------------------------------------------------------------*/
/* Function to broadcast the commitment message                                                  */
/*-----------------------------------------------------------------------------------------------*/
void broadcast() {
    // try to broadcast every 0.5 s
    if(kilo_ticks > last_broadcast_ticks + BROADCAST_TICKS && init_flag){
        last_broadcast_ticks = kilo_ticks;

        // share commitment with certain probability if the robot is committed
        unsigned int range_rnd = 10000;
        unsigned int random = GetRandomNumber(range_rnd);

        unsigned int p_share_commitment_int;
        if (robot_commitment_quality == 0.0){
            p_share_commitment_int =
                    (unsigned int) ((2 * robot_commitment_quality_tmp) * range_rnd) + 1;
        } else {
            p_share_commitment_int =
                    (unsigned int) ((2 * robot_commitment_quality) * range_rnd) + 1;
        }

        // broadcast message if the robot is committed - with probability equal to commitment quality
        if (robot_commitment != UNCOMMITTED && random <= p_share_commitment_int){
            if((message = kilob_message_send()) != NULL) {
                message->type = 67;
                message->data[0] = robot_commitment;
                // set_color(RGB(0,3,0));
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*/
/* Function for setting the motor speed.                                                         */
/*-----------------------------------------------------------------------------------------------*/
void set_motion(){
    // only change if motion type changed
    if(last_state != current_state){
        last_state = current_state;
        // debug_counter = debug_counter + 1;
        // if (debug_counter % 2 == 0){
        //     set_color(RGB(0,3,0));
        // }else {
        //     set_color(RGB(3,0,0));
        // }
        

        spinup_motors();
        switch(current_state) {
            case MOVE_STRAIGHT:
                if (CALIBRATED) set_motors(kilo_straight_left, kilo_straight_right);
                //else set_motors(67, 67);
                break;
            case AVOIDANCE_STRAIGHT:
                if (CALIBRATED) set_motors(kilo_straight_left, kilo_straight_right);
                //else set_motors(67, 67);
                break;
            case MOVE_LEFT:
                if (CALIBRATED) set_motors(kilo_turn_left, 0);
                //else set_motors(70, 0);
                break;
            case MOVE_RIGHT:
                if (CALIBRATED) set_motors(0, kilo_turn_right);
                //else set_motors(0, 70);
                break;
            case AVOIDANCE_TURN_LEFT:
                if (CALIBRATED){
                    // case robot got stuck at border 
                    if (robot_hit_wall) {
                        set_motors(kilo_turn_left + GetRandomNumber(10000) % 30, 0);
                    } else {
                        set_motors(kilo_turn_left, 0);
                    }    
                } 
                //else set_motors(70 + GetRandomNumber(10000) % 30, 0);
                break;
            case AVOIDANCE_TURN_RIGHT:
                if (CALIBRATED){
                    // case robot got stuck at border 
                    if (robot_hit_wall){
                        set_motors(0, kilo_turn_right + GetRandomNumber(10000) % 30);
                    } else {
                        set_motors(0, kilo_turn_right);
                    }
                } 
                //else set_motors(0, 70 + GetRandomNumber(10000) % 30);
                break;

            case STOP:
            default:
                set_motors(0, 0);
        }
    }
}


/*-----------------------------------------------------------------------------------------------*/
/* Function implementing the uncorrelated random walk with the random waypoint model.            */
/*-----------------------------------------------------------------------------------------------*/
void random_walk_waypoint_model(){
    do {
        // getting a random number in the range [1,GPS_maxcell-1] to avoid the border cells (upper bound is -2 because index is from 0)
        goal_gps_x = GetRandomNumber(10000)%( CELLS_X - 4 ) + 2;
        goal_gps_y = GetRandomNumber(10000)%( CELLS_Y - 4 ) + 2;
        // goal_gps_x = 1;
        // goal_gps_y = 1;
        // if(abs(robot_gps_x - goal_gps_x) >= 4 || abs(robot_gps_y - goal_gps_y) >= 4){
            // if the selected cell is enough distant from the current location, it's good
            break;
        // }
    } while(true);
}


/*-----------------------------------------------------------------------------------------------*/
/* Implements the movement of the robot.                                                         */
/*-----------------------------------------------------------------------------------------------*/
void move(){
    // reached goal - select new waypoint
    if((goal_gps_x == robot_gps_x && goal_gps_y == robot_gps_y) || last_waypoint_time >= kilo_ticks + MAX_WAYPOINT_TIME){
        last_waypoint_time = kilo_ticks;
        random_walk_waypoint_model();
    }

    // select current state
    if((robot_hit_wall || robot_hit_semi_wall) && !(current_state == AVOIDANCE_STRAIGHT || current_state == AVOIDANCE_TURN_LEFT || current_state == AVOIDANCE_TURN_RIGHT)){
        // escape from wall triggered - even if the robot hit the semi wall but it is still allowed
        //  to sample
        int32_t angletogoal = normalize_angle(
                atan2((CELLS_Y/2) - robot_gps_y, (CELLS_X/2) - robot_gps_x) / PI * 180 -
                robot_orientation);
        // right from target
        if (angletogoal <= 0) {
            current_state = AVOIDANCE_TURN_RIGHT;
        } else {  // if (angletogoal > 0) {
            current_state = AVOIDANCE_TURN_LEFT;
        }
        state_counter = (uint32_t) ( fabs(angletogoal)/ROTATION_SPEED*32.0 );  // it is important to keep this as fabs bc abs introduces error that the robtos only turn on the spot

    }else if(current_state == AVOIDANCE_TURN_LEFT || current_state == AVOIDANCE_TURN_RIGHT){
        // avoidance turn -> avoidance straight
        state_counter = state_counter - 1;
        if(state_counter == 0){
            current_state = AVOIDANCE_STRAIGHT;
            state_counter = GetRandomNumber(10000) % AVOIDANCE_STRAIGHT_COUNTER_MAX + AVOIDANCE_STRAIGHT_COUNTER_MAX/2;
        }
    }else if(current_state == AVOIDANCE_STRAIGHT){
        // avoidance straight -> move straight
        state_counter = state_counter - 1;
        if(state_counter == 0){
            current_state = MOVE_STRAIGHT;
        }
    }else if(current_state == MOVE_LEFT || current_state == MOVE_RIGHT){
        // turning -> move straight
        state_counter = state_counter - 1;
        if(state_counter == 0){
            current_state = MOVE_STRAIGHT;
            state_counter = STRAIGHT_COUNTER_MAX + GetRandomNumber(10000) % (STRAIGHT_COUNTER_MAX/2);
        }
    }else if(current_state == MOVE_STRAIGHT){
        if(state_counter != 0){
            state_counter = state_counter - 1;
        }else{
            if (calculated_orientation) {
                calculated_orientation = false;
                // check if robot needs to turn
                // see if we are on track
    //             bool right_direction = false; // flag set if we move towards the right celestial direction
    //             if (robot_gps_y == goal_gps_y && robot_gps_x < goal_gps_x && robot_orientation == 0) {
    //                 right_direction = true;
    //             } else if (robot_gps_y > goal_gps_y && robot_gps_x < goal_gps_x &&
    //                        robot_orientation == -45) {
    //                 right_direction = true;
    //             } else if (robot_gps_y > goal_gps_y && robot_gps_x == goal_gps_x &&
    //                        robot_orientation == -90) {
    //                 right_direction = true;
    //             } else if (robot_gps_y > goal_gps_y && robot_gps_x > goal_gps_x &&
    //                        robot_orientation == -135) {
    //                 right_direction = true;
    //             } else if (robot_gps_y == goal_gps_y && robot_gps_x > goal_gps_x &&
    //                        (robot_orientation == -180 || robot_orientation == 180)) {
    //                 right_direction = true;
    //             } else if (robot_gps_y < goal_gps_y && robot_gps_x > goal_gps_x &&
    //                        robot_orientation == 135) {
    //                 right_direction = true;
    //             } else if (robot_gps_y < goal_gps_y && robot_gps_x == goal_gps_x &&
    //                        robot_orientation == 90) {
    //                 right_direction = true;
    //             } else if (robot_gps_y < goal_gps_y && robot_gps_x < goal_gps_x &&
    //                        robot_orientation == 45) {
    //                 right_direction = true;
    //             } else {
    //                 // in this case the robot needs to turn
    // //                printf("[%d] ERROR: robot orientation is of \n", kilo_uid);
    //             }

                // if we are not in the right direction -> turn
                // if (!right_direction) {
                    int32_t angletogoal = normalize_angle(atan2(goal_gps_y - robot_gps_y, goal_gps_x - robot_gps_x) / PI * 180 - robot_orientation);
                    // right from target
                    if (angletogoal < 0) {
                        current_state = MOVE_RIGHT;
                    } else if (angletogoal > 0) {
                        current_state = MOVE_LEFT;
                    } else {
                        printf("[%d] ERROR: turning calculation is fraud \n", kilo_uid);
                    }
                    state_counter = (uint32_t) ( (fabs(angletogoal)/ROTATION_SPEED)*32.0 ); // it is important to keep this as fabs bc abs introduces error that the robtos only turn on the spot
                }
            // }else{
            //     current_state = MOVE_STRAIGHT;
            // }
        }
    }else{
        printf("[%d] ERROR: bad movement state \n", kilo_uid);
    }
    set_motion();
    _delay_ms(20);  // needed because otherwise one cycle would not take 30 ms -> counter would not work
}


/*-----------------------------------------------------------------------------------------------*/
/* Function to process the data received from the kilogrid regarding the environment             */
/*-----------------------------------------------------------------------------------------------*/
void update_grid_msg() {
    // check for obstacles - lock that only one obstacle is viable
    if (received_role == 21){
        robot_hit_semi_wall = true;
        robot_hit_wall = false;
    }else if(received_role == 42){
        robot_hit_wall = true;
        robot_hit_semi_wall = false;
    }else{
        robot_hit_wall = false;
        robot_hit_semi_wall = false;
        current_ground = received_ground;
    }

    // check for position update
    if((robot_gps_x != received_x || robot_gps_y != received_y) && kilo_ticks > STRAIGHT_COUNTER_MAX + last_orientation_update){
        last_orientation_update = kilo_ticks;
        robot_gps_x_last = robot_gps_x;
        robot_gps_y_last = robot_gps_y;
        robot_gps_x = received_x;
        robot_gps_y = received_y;

        // update orientation
        double angleOrientation = atan2(robot_gps_y-robot_gps_y_last, robot_gps_x-robot_gps_x_last)/PI*180;
        robot_orientation = normalize_angle(angleOrientation);
        calculated_orientation = true;
    }
}


/*-----------------------------------------------------------------------------------------------*/
/* Function to process the data received from the kilogrid regarding other robots                */
/*-----------------------------------------------------------------------------------------------*/
void update_virtual_agent_msg() {
    //if(received_kilo_uid != kilo_uid){
        received_option = received_option_msg;
      //  received_kilo_uid = kilo_uid;
        // needs to be set here because we only get a new msg when we update!
        new_robot_msg = true;
        //set_color(RGB(3,0,0));
        
    //}
}


/*-----------------------------------------------------------------------------------------------*/
/* This function implements the callback, for when the robot receives an infrared message (here  */
/* only from the kilogrid)                                                                       */
/*-----------------------------------------------------------------------------------------------*/
// because there has been an "updated" version of the kilo_lib we have a slightly different
// implementation
void message_rx( IR_message_t *msg, distance_measurement_t *d ) {

    if(msg->type == 67 && init_flag){
        received_option_msg = msg->data[0];
        received_virtual_agent_msg_flag = true;
    }else if(msg->type == 11){
        // this message type is send by the kilogrid
        received_x = msg->data[0];
        received_y = msg->data[1];
        received_ground = msg->data[2];
        received_role = msg->data[3];
        received_grid_msg_flag = true;
        init_flag = true;
    }
}


/*-----------------------------------------------------------------------------------------------*/
/* Callback function for successful transmission                                                 */
/*-----------------------------------------------------------------------------------------------*/
void tx_message_success() {
    msg_counter_sent += 1;
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
    
}





/*-----------------------------------------------------------------------------------------------*/
/* Init function                                                                                 */
/*-----------------------------------------------------------------------------------------------*/
void setup(){
    // for tracking the robot in real life
    kilob_tracking_init();
    kilob_messaging_init();
    tracking_data.byte[0] = kilo_uid;
    //tracking_data.byte[5] = 0;

    // Initialise random seed
    uint8_t seed = rand_hard();
    rand_seed(seed);
    seed = rand_hard();
    srand(seed);

    // Initialise motors
    set_motors(0,0);

    random_walk_waypoint_model();

    // used to be in init message 
    robot_commitment = INITIAL_ROBOT_COMMITMENT;
    robot_commitment_quality = 204.0/255.0;
    op_to_sample = 1;
    //communication_range = 2;
    sample_counter_max_noise = SAMPLE_COUNTER_MAX;
        

    //received_kilo_uid = kilo_uid;
    // init some counters
    // TODO: is commented out due to experiments for ants paper with different sampling numbers
    sample_counter_max_noise = SAMPLE_COUNTER_MAX; // + ((GetRandomNumber(10000) % ((uint8_t)(SAMPLE_COUNTER_MAX/10) + 1)) - (uint8_t)(SAMPLE_COUNTER_MAX/20));
    // TODO: does this makes sense? -> update is only for shuffleing so you do not have to wait
    //  a full cycle
    update_ticks_noise = (GetRandomNumber(10000) % UPDATE_TICKS + 1);

    last_broadcast_ticks = GetRandomNumber(10000) % BROADCAST_TICKS + 1;
    last_sample_ticks = GetRandomNumber(10000) % 32 + 1;

    // Initialise time to 0
    kilo_ticks = 0;
}


/*-----------------------------------------------------------------------------------------------*/
/* Main loop                                                                                     */
/*-----------------------------------------------------------------------------------------------*/
void loop() {
    if(init_flag){  // initalization happend
        // process received msgs
        if (received_grid_msg_flag) {
            update_grid_msg();
            received_grid_msg_flag = false;
        }

        if (received_virtual_agent_msg_flag) {
            update_virtual_agent_msg();
            received_virtual_agent_msg_flag = false;
        }

        // move robot
        move();

        // sample - current ground for getting your own opinion
        sample();

        // update commitment - of the robot
        update_commitment();

        // update communication range
        update_communication_range();

        // set broadcast if needed
        broadcast();

        switch(robot_commitment) {
            case 0:
                set_color(RGB(0,0,0));
                break;
            case 1:
                set_color(RGB(0,0,3));
                break;
            case 2:
                set_color(RGB(3,3,0));
                break;
            // case 3:
            //     set_color(RGB(3,0,0));
            //     break;
            // case 4:
            //     set_color(RGB(0,3,3));
            //     break;
            // case 5:
            //     set_color(RGB(0,0,0));
            //     break;
            default:
                //printf("[%d] ERROR - wrong state %d \n", kilo_uid, robot_commitment);
                set_color(RGB(3,3,3));
                break;
        }
        // if (current_state == MOVE_STRAIGHT){
        //     set_color(RGB(0,3,0));
        // }else if (current_state == MOVE_LEFT){
        //     set_color(RGB(3,0,0));
        // }else if (current_state == MOVE_RIGHT){
        //     set_color(RGB(0,0,3));
        // }else{
        //     set_color(RGB(0,0,0));
        // }
    }else{
        // not initialized yet ... can be omitted just for better understanding
        // also you can do some debugging here
        set_color(RGB(3,3,3));
    }

    // tracing data
    tracking_data.byte[1] = received_x;
    tracking_data.byte[2] = received_y;
    tracking_data.byte[3] = robot_commitment;
    tracking_data.byte[4] = goal_gps_x;
    tracking_data.byte[5] = goal_gps_y;
    kilob_tracking(&tracking_data);
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