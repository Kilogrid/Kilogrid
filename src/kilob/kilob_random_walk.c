#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "kilob_random_walk.h"
#include "kilo_rand_lib.h"
#include "utils.h"

typedef enum{
	KILOB_RANDOM_WALK_IDLE = 0x00,
	KILOB_RANDOM_WALK_INIT,
	KILOB_RANDOM_WALK_STRAIGHT,
	KILOB_RANDOM_WALK_ROTATION
} kilob_random_walk_state_t;

/** @brief State of the random walk behavior. */
static kilob_random_walk_state_t behavior_state = KILOB_RANDOM_WALK_IDLE;

/*** Random Walk constants ****/
static double forward_mu_sec  		= 3.0;
static double forward_sigma_sec		= 1.5;
static double rotation_const_sec	= 1.5;
static double rotation_span_sec		= 3.0;

/** @brief Timer used by the random walk behavior. */
static uint32_t kilob_rw_timer	= 0;

/**
 * @brief Implements random walk for the kilobot. Should be called from the main loop() function.
 *
 *	- KILOB_RANDOM_WALK_INIT and any other state not detailed here:
 *		This is the entry state, initializes a timeout for straight movement.
 *
 *	- KILOB_RANDOM_WALK_STRAIGHT:
 *		Entered from BEHAVIOR_STATE_RANDOM_WALK_INIT instantly, if the timeout is reached
 *		it chooses a direction and a new timeout randomly and starts turning in the choosen
 *		direction. Enters the state KILOB_RANDOM_WALK_ROTATION.
 *
 *	- KILOB_RANDOM_WALK_ROTATION:
 *		Entered from KILOB_RANDOM_WALK_STRAIGHT, waits for the amount of time
 *		choosen in the previous step and returns to the KILOB_RANDOM_WALK_INIT
 *		state after for reinitialization.
 */
void kilob_random_walk() {
	uint32_t tmp = 0;
	double tmp_uniform = 0;

	switch(behavior_state) {
		case KILOB_RANDOM_WALK_STRAIGHT:
			if(kilo_ticks > kilob_rw_timer) { // time to end straight motion

				tmp = INT_GAUSSIAN_TICKS(rotation_const_sec, rotation_span_sec);

				tmp_uniform = g_ran_uniform(); // between 0 and 1

				//double time_span_sec = RW_ROTATION_MAX_TIME_SEC - RW_ROTATION_MIN_TIME_SEC; // e.g. max = 5s, min = 1s: time_span = 4s, and the actual time of rotation will go from 1s to (1+4) = 5s depending on the outcome of g_ran_uniform().

				//tmp = (uint32_t)( time_span_sec*tmp_uniform + RW_ROTATION_MIN_TIME_SEC ); // compute actual time of rotation, in seconds

				//tmp = SEC_TO_TICKS(tmp); // convert to the Kilobot scale

				kilob_rw_timer = kilo_ticks + tmp; // update timer variable

				if (tmp_uniform > 0.5) {
					#ifdef ENABLE_LIGHTS_RW_WA
					set_color(RGB(0,1,0)); // green
					#endif
					turn_left();
				}
				else {
					#ifdef ENABLE_LIGHTS_RW_WA
					set_color(RGB(0,0,1)); // blue
					#endif
					turn_right();
				}

				behavior_state = KILOB_RANDOM_WALK_ROTATION;
			}
			break;

		case KILOB_RANDOM_WALK_ROTATION:
			if(kilo_ticks > kilob_rw_timer) {
				behavior_state = KILOB_RANDOM_WALK_INIT;
			}
			break;

		case KILOB_RANDOM_WALK_INIT:
		default:

			tmp = INT_GAUSSIAN_TICKS(forward_mu_sec, forward_sigma_sec);

			kilob_rw_timer = kilo_ticks + tmp;

			move_straight();
			#ifdef ENABLE_LIGHTS_RW_WA
			set_color(RGB(0,0,0));
			#endif

			behavior_state = KILOB_RANDOM_WALK_STRAIGHT;
			break;
	}
}

/**
 * @brief Initializes the state to KILOB_RANDOM_WALK_INIT. The rest of the initialization
 * is done in kilob_random_walk().
 */
void kilob_random_walk_init(double _forward_mu_sec, double _forward_sigma_sec,
	double _rotation_const_sec, double _rotation_span_sec) {

	forward_mu_sec = _forward_mu_sec;
	forward_sigma_sec = _forward_sigma_sec;
	rotation_const_sec = _rotation_const_sec;
	rotation_span_sec = _rotation_span_sec;

	behavior_state = KILOB_RANDOM_WALK_INIT;
}

/**
 * @brief Reset random walk state
 */
void kilob_random_walk_reset() {
    behavior_state = KILOB_RANDOM_WALK_INIT;
}
