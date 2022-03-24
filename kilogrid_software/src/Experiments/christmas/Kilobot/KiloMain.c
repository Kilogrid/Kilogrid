#include <stdlib.h>

#include "kilolib.h"
#include "utils.h"
#include <math.h>
#include "kilob_tracking.h"
#include "kilo_rand_lib.h"
#include "../communication.h"
#include "kilob_messaging.h"
//#include "kilob_random_walk.h"

#define PI 3.14159265358979323846
#define GRID_MSG 1

/*-----------------------------------------------------------------------------------------------*/
/* Enums section                                                                                 */
/*-----------------------------------------------------------------------------------------------*/
// Enum for different motion types
typedef enum {
    STOP = 0,
    FORWARD,
    TURN_LEFT_MY,
    TURN_RIGHT_MY
} motion_t;

// Enum for boolean flags
typedef enum {
    false = 0,
    true = 1
} bool;

// Emum for new way point calculation
typedef enum {
    SELECT_NEW_WAY_POINT = 1,
    UPDATE_WAY_POINT = 0
} waypoint_option;


/*-----------------------------------------------------------------------------------------------*/
/* Robot state                                                                                   */
/*-----------------------------------------------------------------------------------------------*/
// robot state variables
uint8_t Robot_GPS_X;  // current x position
uint8_t Robot_GPS_Y;  // current y position
uint32_t Robot_orientation;  // current orientation
uint8_t Robot_GPS_X_last;  // last x position  -> needed for calculating the orientation
uint8_t Robot_GPS_Y_last;  // last y position  -> needed for calculating the orientation
motion_t current_motion_type = STOP;  // current type of motion

// robot goal variables
uint8_t Goal_GPS_X = 5;  // x pos of goal
uint8_t Goal_GPS_Y = 18;  // y pos of goal
bool update_orientation = false;
uint32_t update_orientation_ticks = 0;
const uint32_t UPDATE_ORIENTATION_MAX_TICKS = 32;

const uint8_t MIN_DIST = 4;  // min distance the new way point has to differ from the last one
const uint32_t MAX_WAYPOINT_TIME = 3600; // about 2 minutes -> after this time choose new way point
uint32_t lastWaypointTime;  // time when the last way point was chosen

// stuff for motion
const bool CALIBRATED = true;  // flag if robot is calibrated??
const uint8_t TURNING_TICKS = 60; /* constant to allow a maximum rotation of 180 degrees with \omega=\pi/5 */
const uint8_t MANY_TURNING_TICKS = 130;
const uint32_t MAX_STRAIGHT_TICKS = 150;

uint32_t turning_ticks = 0;  // TODO chnage back to unsigned int?????
uint32_t last_motion_ticks = 0;
uint32_t straight_ticks = 0;

// Wall Avoidance manouvers
uint32_t wallAvoidanceCounter = 0; // to decide when the robot is stuck...
bool stuck = false;  // needed if the robot gets stuck
bool hit_wall = false;

tracking_user_data_t tracking_data;


/*-----------------------------------------------------------------------------------------------*/
/* Arena variables                                                                               */
/*-----------------------------------------------------------------------------------------------*/
const uint8_t GPS_MAX_CELL_X = 10;
const uint8_t GPS_MAX_CELL_Y = 20;

// communication
uint8_t received_x = 0;
uint8_t received_y = 0;
uint8_t received_role = 0;
bool received_msg_kilogrid = false;

// dirty hack
bool init = true;
bool finished = false;



/*-----------------------------------------------------------------------------------------------*/
/* Normalizes angle between -180 < angle < 180.                                                  */
/*-----------------------------------------------------------------------------------------------*/
void NormalizeAngle(double* angle){
    while(*angle>180){
        *angle=*angle-360.0;
    }
    while(*angle<-180){
        *angle=*angle+360.0;
    }
}

double normalize_angle(double angle){
    if(angle>180){
        angle=angle-360.0;
    }
    if(angle<-180){
        angle=angle+360.0;
    }
    return angle;
}


/*-----------------------------------------------------------------------------------------------*/
/* Function for setting the motor speed.                                                         */
/*-----------------------------------------------------------------------------------------------*/
void set_motion(motion_t new_motion_type) {
    // only update current motion if change
    if(current_motion_type != new_motion_type){
        current_motion_type = new_motion_type;

        switch( new_motion_type ){
            case FORWARD:
                spinup_motors();
                if (CALIBRATED) set_motors(kilo_straight_left,kilo_straight_right);
                else set_motors(67,67);
                break;
            case TURN_LEFT_MY:
                spinup_motors();
                if (CALIBRATED){
                    uint8_t leftStrenght = kilo_turn_left;
                    uint8_t i;
                    for (i=3; i <= 18; i += 3){
                        if (wallAvoidanceCounter >= i){
                            leftStrenght+=2;
                        }
                    }
                    set_motors(leftStrenght,0);
                }else{
                    set_motors(70,0);
                }
                break;
            case TURN_RIGHT_MY:
                spinup_motors();
                if (CALIBRATED){
                    uint8_t rightStrenght = kilo_turn_right;
                    uint8_t i;
                    for (i=3; i <= 18; i += 3){
                        if (wallAvoidanceCounter >= i){
                            rightStrenght+=2;
                        }
                    }
                    set_motors(0,rightStrenght);
                }
                else{
                    set_motors(0,70);
                }
                break;
            case STOP:
            default:
                set_motors(0,0);
        }
    }
}



/*-----------------------------------------------------------------------------------------------*/
/* Function to check if the robot is aganst the wall                                             */
/*-----------------------------------------------------------------------------------------------*/
void check_if_against_a_wall() {
    // handles target when we are at a wall?
    if(hit_wall){
        // drive towards the map center if coliding with a wall??
        if (wallAvoidanceCounter<18) wallAvoidanceCounter += 1;
       else wallAvoidanceCounter = 1;
    } else {
        if (wallAvoidanceCounter > 0){ // flag when the robot hit a wall -> after select new waypoint -> else ignore
            //random_walk_waypoint_model(SELECT_NEW_WAY_POINT);
            // drive towards the center 
            Goal_GPS_X = GPS_MAX_CELL_X/2;
            Goal_GPS_Y = GPS_MAX_CELL_Y/2;
        }
        wallAvoidanceCounter = 0;
    }
}


/*-----------------------------------------------------------------------------------------------*/
/* Function to go to the Goal location, called every cycle,                                      */
/*-----------------------------------------------------------------------------------------------*/
void GoToGoalLocation() {
    // only recalculate movement if the robot has an update on its orientation aka. moved
    // if hit wall do the hit wall case... also stuck ensures that they drive straight forward for
    // a certain amount of time
    if (update_orientation && !hit_wall && !stuck){
        update_orientation = false;
        
        // calculates the difference thus we can see if we have to turn left or right
        double angletogoal = atan2(Goal_GPS_Y-Robot_GPS_Y,Goal_GPS_X-Robot_GPS_X)/PI*180-Robot_orientation;
        angletogoal = normalize_angle(angletogoal);
        
        // see if we are on track
        bool right_direction = false; // flag set if we move towards the right celestial direction
        if(Robot_GPS_Y == Goal_GPS_Y && Robot_GPS_X < Goal_GPS_X){ // right case
            if(Robot_orientation == 0){ right_direction = true;}
        }else if (Robot_GPS_Y > Goal_GPS_Y && Robot_GPS_X < Goal_GPS_X){  // bottom right case
            if(Robot_orientation == -45){ right_direction = true;}
        }else if (Robot_GPS_Y > Goal_GPS_Y && Robot_GPS_X == Goal_GPS_X){  // bottom case
            if(Robot_orientation == -90){ right_direction = true;}
        }else if (Robot_GPS_Y > Goal_GPS_Y && Robot_GPS_X > Goal_GPS_X){  // bottom left case
            if(Robot_orientation == -135){ right_direction = true;}
        }else if (Robot_GPS_Y == Goal_GPS_Y && Robot_GPS_X > Goal_GPS_X){  // left case
            if(Robot_orientation == -180 || Robot_orientation == 180){ right_direction = true;}
        }else if (Robot_GPS_Y < Goal_GPS_Y && Robot_GPS_X > Goal_GPS_X){  // left upper case
            if(Robot_orientation == 135){ right_direction = true;}
        }else if (Robot_GPS_Y < Goal_GPS_Y && Robot_GPS_X == Goal_GPS_X){  // upper case
            if(Robot_orientation == 90){ right_direction = true;}
        }else if (Robot_GPS_Y < Goal_GPS_Y && Robot_GPS_X < Goal_GPS_X){  // right upper case
            if(Robot_orientation == 45){ right_direction = true;}
        }else{
            //printf("[%d] ERROR - something wrong in drive cases \n", kilo_uid);
        }
        
        // if we are not in the right direction -> turn
        if (!right_direction){
            // right from target
            if ((int)angletogoal < 0 ) set_motion(TURN_RIGHT_MY);
            // left from target
            else if ((int)angletogoal > 0) set_motion(TURN_LEFT_MY);
            turning_ticks= TURNING_TICKS;
            last_motion_ticks = kilo_ticks;

        }else{
            set_motion(FORWARD);
        }
    }else if (hit_wall){
        // case that we hit a wall: first turn away, than drive straight for a while
        if (stuck){
            set_motion(FORWARD);
        }else if(!(current_motion_type == TURN_LEFT_MY || current_motion_type == TURN_RIGHT_MY)){
            double aTC =atan2(GPS_MAX_CELL_Y/2-Robot_GPS_Y,GPS_MAX_CELL_X/2-Robot_GPS_X)/PI*180-Robot_orientation;
            aTC = normalize_angle(aTC);
            if (aTC > 0){
                set_motion(TURN_LEFT_MY);
                last_motion_ticks = kilo_ticks;
                //turning_ticks = (unsigned int) (fabs(aTC)/38.0*32.0);  // /38.0*32.0
            }else{
                set_motion(TURN_RIGHT_MY);
                last_motion_ticks = kilo_ticks;
                //turning_ticks = (unsigned int) (fabs(aTC)/38.0*32.0);  // /38.0*32.0
            }
            turning_ticks = rand() % 75 + 70; // should be max 180 deg turn aka 4.5 sec 2 bis 4 sec drehen?
            stuck = false;
        }
    }

    // needed at least for the beginning so that the robot starts moving into the right direction
    // but also to update after turning for a while -> move then straight to update orientation
    switch( current_motion_type ) {
        case TURN_LEFT_MY:
        case TURN_RIGHT_MY:
            if( kilo_ticks > last_motion_ticks + turning_ticks) {
                /* start moving forward */
                last_motion_ticks = kilo_ticks;  // fixed time FORWARD
                if (hit_wall){  // only enforce straight movement when trying to escape a wall
                    straight_ticks = rand() % MAX_STRAIGHT_TICKS + 150;
                    stuck = true;
                }
                set_motion(FORWARD);
            }
            break;
        case FORWARD:
            if(kilo_ticks > last_motion_ticks + straight_ticks){
                last_motion_ticks = kilo_ticks;
                stuck = false;
            }
            break;

        case STOP:
        default:
            set_motion(STOP);
    }
}

void update_robot_state(){
    // update reading of sensor - also check for hit_wall flag - setting it inside callback
    // function could lead to inconsistancy!!

    // update current and last position
    if (kilo_ticks > update_orientation_ticks + UPDATE_ORIENTATION_MAX_TICKS){
        update_orientation_ticks = kilo_ticks;
        if (received_role == 42){  // robot sensed wall -> dont update the received option
            hit_wall = true;
            tracking_data.byte[5] = 1;
        }else{
            hit_wall = false;
            // update goal bc it may get resetted in the escape behavior
            Goal_GPS_X = 5;
            Goal_GPS_Y = 19;
        }
        if ((received_x != Robot_GPS_X || received_y != Robot_GPS_Y) && (received_x!=Robot_GPS_X_last || received_y != Robot_GPS_Y_last)){
            Robot_GPS_X_last = Robot_GPS_X;
            Robot_GPS_X = received_x;
            Robot_GPS_Y_last = Robot_GPS_Y;
            Robot_GPS_Y = received_y;
            update_orientation = true;
           // set_color(RGB(0,0,3));
           // delay(200);
           // set_color(RGB(0,0,0));

            // calculate orientation of the robot based on the last and current visited cell -> rough estimate
            double angleOrientation = atan2(Robot_GPS_Y-Robot_GPS_Y_last, Robot_GPS_X-Robot_GPS_X_last)/PI*180;
            angleOrientation = normalize_angle(angleOrientation);
            Robot_orientation = (uint32_t) angleOrientation;
        }
    }
    if(init){
        init = false;
        Goal_GPS_X = received_x;
    }


    received_msg_kilogrid = false;
}

/*-----------------------------------------------------------------------------------------------*/
/* Init function                                                                                 */
/*-----------------------------------------------------------------------------------------------*/
void setup(){
    kilob_tracking_init();
    tracking_data.byte[0] = kilo_uid;
    // Initialise motors
    set_motors(0,0);

    set_motion(FORWARD);

    // Initialise motion variables
    last_motion_ticks = rand() % MAX_STRAIGHT_TICKS + 1;

    set_color(RGB(3,3,3));

    // init robot state
    Robot_GPS_X_last = GPS_MAX_CELL_X/2;
    Robot_GPS_Y_last = GPS_MAX_CELL_Y/2;
    Robot_orientation = 0;
    // initialise the GSP to the middle of the environment, to avoid to trigger wall avoidance immediately
    Robot_GPS_X = GPS_MAX_CELL_X/2;
    Robot_GPS_Y = GPS_MAX_CELL_Y/2;

    // Intialize time to 0
    kilo_ticks = 0;
}


/*-------------------------------------------------------------------*/
/* Callback function for message reception                           */
/*-------------------------------------------------------------------*/
void message_rx( IR_message_t *msg, distance_measurement_t *d ) {
    if (msg->type == 1){
        received_x = msg->data[0];
        received_y = msg->data[1];
        received_role = msg->data[2];
        received_msg_kilogrid = true;
        //set_color(RGB(3,0,0));
        //delay(200);
        //set_color(RGB(0,0,0));
    }
    return;
}

/*-----------------------------------------------------------------------------------------------*/
/* Callback function for message transmission - this methods do not need to be implemented for   */
/* the simulation bc inter robot communication is handled by the kilogrid                        */
/*-----------------------------------------------------------------------------------------------*/
IR_message_t *message_tx() {
    return NULL;
}

/*-----------------------------------------------------------------------------------------------*/
/* Callback function for successful transmission  - this methods do not need to be implemented   */
/* for the simulation bc inter robot communication is handled by the kilogrid                    */
/*-----------------------------------------------------------------------------------------------*/
void tx_message_success() {
    return;
}

/*-----------------------------------------------------------------------------------------------*/
/* Main loop                                                                                     */
/*-----------------------------------------------------------------------------------------------*/
void loop() {
    // goal reached
    if(received_y >= 18 || finished){
        finished = true;
        set_motors(0,0);
        set_color(RGB(3,3,3));
        delay(100);
        set_color(RGB(0,0,0));
        delay(100);
    }else{
        // main loop
        if(received_msg_kilogrid){
            update_robot_state();
            check_if_against_a_wall();

            //set_color(RGB(3,0,0));
        }

        // move towards random location
        GoToGoalLocation();
        /*
        if(hit_wall){
            set_color(RGB(0,3,0));
        }else{
            set_color(RGB(3,3,3));
        }
        
        switch(current_motion_type){
            case FORWARD:
                set_color(RGB(0,3,0));
                break;
            case TURN_RIGHT_MY:
                set_color(RGB(3,0,0));
                break;
            case TURN_LEFT_MY:
                set_color(RGB(0,0,3));
                break;
            case STOP:
            default:
                set_color(RGB(3,3,3));

        }
*/
        //set_color(RGB(3,3,3));
    }
    tracking_data.byte[1] = received_x;
    tracking_data.byte[2] = received_y;
    tracking_data.byte[3] = Robot_GPS_X;
    tracking_data.byte[4] = Robot_GPS_Y;
    kilob_tracking(&tracking_data);

}

/*-----------------------------------------------------------------------------------------------*/
/* Main function                                                                                 */
/*-----------------------------------------------------------------------------------------------*/
int main(){
    kilo_init();  // init hardware of the kilobot
    utils_init();
    kilo_message_tx = message_tx;  // register callback - pointer to message which should be send - roughly every 0.5 s
    kilo_message_tx_success = tx_message_success;  // triggered when transmission is successfull
    kilo_message_rx = message_rx;  // callback for received messages

    kilo_start(setup, loop);
    return 0;
}
