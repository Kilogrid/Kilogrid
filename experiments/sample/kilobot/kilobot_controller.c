/**
 * @file     Kilogrid/experiments/sample/kilobot/kilobot_controller.c
 * @brief    This file implements a sample code of controller for the Kilobot.
 *
 * @details    The controller is composed of two main functions:
 *  - setup : in order to setup at the starts of the robot ...
 * [TODO take inspiration from arduino & argos]
 *  - loop : a main loop that runs indefinetly
 * The user can then access different function via the provided libraries
 * (e.g. tracking, random, etc...) please refer to the documentation for 
 * more details.
 
 * [TODO : adding author, date, and copyright each time does not sounds relevant]
 * @author   IRIDIA lab
 * @date     March, 2017
 * @copyright licensed under creative commons attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0) 
 *           more info at http://creativecommons.org/licenses/by-nc-sa/3.0/
 */

#include <kilolib.h>
#include <utils.h>           // required for utils_init, utils_message_rx
#include <kilob_tracking.h>  // required for kilob_tracking


/** Tracking data container */
static tracking_user_data_t tracking_data;


/**
 * @brief IR message receiving callback.
 *
 * @param m  the message
 * @param d  the estimated distance between itself and message sender
 */
void ir_rx(IR_message_t *m, distance_measurement_t *d) {
    /* [TODO] Put you code */
}


/**
 * @brief Setup kilobot controller once at start of the robot.
 */
void setup() {
    // initialize tracking function
	kilob_tracking_init();

    // initialize tracking data
	for(uint8_t i = 0; i < TRACKING_MSG_USER_DATA; i++) {
		tracking_data.byte[i] = 0;
	}
	tracking_data.byte[0] = kilo_uid;

    /* [TODO] Put your setup code */
}


/**
 * @brief Main loop of kilobot controller.
 */
void loop() {
    /* [TODO] Put you experiment code */

    // send tracking data
	kilob_tracking(&tracking_data);
}


int main() {
    // [Required] initialize kilobot controller
    // This function should be called first.
    kilo_init();

    // [Required] initialize kilobot utils
    // It's required to use several KiloGUI command, turn_left, turn_right,
    // and go_straight.
    utils_init();

    // [Option] register message receiving callback
    utils_message_rx = ir_rx;

    // [Required] start kilobot controller
    // This function should be called at the end of main function.
    kilo_start(setup, loop);
    return 0;
}
