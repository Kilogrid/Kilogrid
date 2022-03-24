#include <stddef.h>
#include <math.h>
#include "kilolib.h"
#include "utils.h"
#include "kilo_rand_lib.h"
#include "kilob_common.h"
#include "kilob_messaging.h"
#include "kilob_gradient.h"
#include "kilob_tracking.h"
#include "kilob_random_walk.h"
#include "kilob_cdms.h"
#include "kilogrid.h"
#include "decision_making_common_defines.h"
#include "message.h"

#define MODULATION_MIXED
#define INITIAL_OPINION 1;

/**
 * IR message type to broadcast the current decision of this robot.
 */
const static uint8_t OPINION_MSG_TYPE = 0x20;

typedef enum {
	INIT 					= 0x00,
	MOVING_TO_NEST_INIT 	= 0x01,
	MOVING_TO_NEST 			= 0x02,
	MOVING_TO_SITE_INIT 	= 0x03,
	MOVING_TO_SITE_A 		= 0x04,
	MOVING_TO_SITE_B		= 0x05,
	RANDOM_WALK_NEST_INIT 	= 0x10,
	RANDOM_WALK_NEST 		= 0x11,
	RANDOM_WALK_SITE_INIT 	= 0x20,
	RANDOM_WALK_SITE 		= 0x21,
	COLLECTING_OPINIONS 	= 0x30
} kilobot_state_t;

volatile kilobot_state_t state;
tracking_user_data_t tracking_data;

/**
 * @brief Opinion of this robot.
 *
 * 0: opinion A
 * 1: opinion B
 */
opinion_t opinion = INITIAL_OPINION;

/**
 * @brief When the robot is moving, the target is specified in this variable.
 *
 * 0: site A
 * 1: site B
 */
uint8_t target_site = 0;

/**
 * @brief Timer used for tracking random walk time in sites and nest.
 */
uint32_t timer = 0;

/**
 * @brief Current quality measurement.
 */
volatile double quality;

/**
 * @brief Number of quality messages received.
 */
volatile uint16_t num_quality;

/**
 * @brief Seconds elapsed while traveling from the nest to the site, and back.
 */
uint32_t distance;

#ifdef MODULATION_INDIRECT
/**
 * @brief Calculates the dissemination time with direct modulation.
 */
inline double dissemination_param() {
	return 1/MODULATION_G_PARAM;
}
#endif

#ifdef MODULATION_DIRECT
/**
 * @brief Calculates the dissemination time with direct modulation.
 */
inline double dissemination_param() {
	return 1/(MODULATION_G_PARAM*
		exp(quality*MODULATION_EXP_SCALING)/exp(MODULATION_EXP_SCALING));
}
#endif

#ifdef MODULATION_MIXED
/**
 * @brief Calculates the dissemination time with mixed modulation.
 */
inline double dissemination_param() {
	return 1/((MODULATION_G_PARAM + distance)*
		exp(quality*MODULATION_EXP_SCALING)/exp(MODULATION_EXP_SCALING));
}
#endif

/** 
 * @brief Encodes the kilobot id and current opinion in the form:
 *		[7:1] - kilobot_id [0..127]
 *		[0]   - opinion
 */
inline uint8_t encode_opinion(uint8_t kid, uint8_t op) {
	// highest bit always 0, because values range from 0 to < 127
	uint8_t ret = kid << 1;
	ret |= op;

	return ret;
}

/**
 * @brief Callback called when the robot escaped the to_site gradient, meaning
 * it arrived at either site A or site B.
 */
void on_escape_to_sites() {}

/**
 * @brief Callback called when the robot escaped the to_nest gradient, meaning
 * it arrived at the nest.
 */
void on_escape_to_nest() {}

/**
 * @brief Callback called when the robot escaped the wall gradient.
 */
void on_escape_wall() {}

/**
 * @brief Reset gradient tracking configuration, enabling only the wall
 * gradient in decreasing direction.
 */
void inline reset_gradient_tracking() {
	kilob_gradient_tracking_reset();
	kilob_gradient_tracking_config_add(GRADIENT_IDX_WALL,
		kilob_gradient_tracking_decreasing, on_escape_wall, 1);
}

void ir_rx(IR_message_t *m, distance_measurement_t *d) {
	if(m->type == OPINION_MSG_TYPE && 
	   (state == COLLECTING_OPINIONS || state == RANDOM_WALK_NEST))
	{
		kilob_cdms_collect(m->data[0], m->data[1], 0.);
	}
	else if(m->type == GRADIENT_MSG_TYPE) {
		if(state == RANDOM_WALK_SITE) {
			quality += (double)m->data[5] / (255.0/2);
			num_quality++;
		}

		switch(state) {
			case INIT:
				if(m->data[4] == AREA_TYPE_NEST) {
					state = MOVING_TO_SITE_INIT;
				}
				else {
					state = RANDOM_WALK_SITE_INIT;
				}
				break;

			case MOVING_TO_SITE_A:
			case MOVING_TO_SITE_B:
				if(m->data[4] == AREA_TYPE_SITE_A || m->data[4] == AREA_TYPE_SITE_B) {
					state = RANDOM_WALK_SITE_INIT;
				}
				break;

			case MOVING_TO_NEST:
				if(m->data[4] == AREA_TYPE_NEST) {
					state = RANDOM_WALK_NEST_INIT;
				}
				break;

			default:
				break;
		}

		kilob_gradient_tracking_message_received(m, d);
	}
}

void setup() {
	g_ran_set_seed_uint32(0);
	kilob_messaging_init();

	kilob_tracking_init();
	tracking_data.byte[0] = encode_opinion(kilo_uid, opinion);

	kilob_gradient_tracking_init(GRADIENT_CHECK_PERIOD, GRADIENT_TURN_SMALL_MU,
		GRADIENT_TURN_SMALL_SIGMA, GRADIENT_TURN_LARGE_MU, GRADIENT_TURN_LARGE_SIGMA,
		GRADIENT_STUCK_TIMEOUT);

	reset_gradient_tracking();
	kilob_cdms_init(2, MAJORITY_RULE_GROUP_SIZE-1); //number of opinions is always 2
	state = INIT;
}

void loop() {
	uint8_t i;
	opinion_info_t op;

	if(opinion == 0) {
		set_color(RGB(0,2,1));
	}
	else {
		set_color(RGB(1,0,1));
	}

	switch(state) {
		/*
		 * Waiting for the first message from the grid to determine the type of area we are in.
		 */
		case INIT:
			break;

		/*
		 * Here we initialize gradient tracking in the direction of the choosen site. 
		 * The wall gradient is added by default. The TO_SITES gradient tracking
		 * direction is dependent on the choosen site.
		 */
		case MOVING_TO_SITE_INIT:
			// choose target site based on opinion
			target_site = opinion;
			reset_gradient_tracking();
			distance = kilo_ticks;

			//set up gradient tracking
			if(target_site == 0) {
				state = MOVING_TO_SITE_A;
				kilob_gradient_tracking_config_add(GRADIENT_IDX_TO_SITE_A,
					kilob_gradient_tracking_decreasing, on_escape_to_sites, 0);
			}
			else {
				state = MOVING_TO_SITE_B;
				kilob_gradient_tracking_config_add(GRADIENT_IDX_TO_SITE_B,
					kilob_gradient_tracking_decreasing, on_escape_to_sites, 0);
			}
			
			break;

		/*
		 * Initialize gradient tracking in order to navigate back to the nest.
		 */
		case MOVING_TO_NEST_INIT:
			state = MOVING_TO_NEST;
			reset_gradient_tracking();
			
			kilob_gradient_tracking_config_add(GRADIENT_IDX_TO_NEST,
				kilob_gradient_tracking_decreasing, on_escape_to_nest, 0);

			if(num_quality == 0) {
				num_quality = 1;
			}

			quality = quality / num_quality;
			tracking_data.byte[5] = num_quality;
			break;

		/*
		 * Initialize the timer for the random walk in the nest based on the
		 * observed quality and the used modulation.
		 */
		case RANDOM_WALK_NEST_INIT:
			state = RANDOM_WALK_NEST;
			distance = (kilo_ticks - distance) / TICKS_PER_SEC;
			timer = kilo_ticks + (uint32_t)g_ran_exponential(dissemination_param()) * TICKS_PER_SEC;

			kilob_random_walk_init(RW_NEST_FORWARD_MU, RW_NEST_FORWARD_SIGMA,
				RW_ROTATION_CONST, RW_ROTATION_SPAN);
			break;

		/*
		 * Wait for dissemination time, then start moving to one of the sites.
		 */
		case RANDOM_WALK_NEST:
			if(kilo_ticks > timer) {
				state = COLLECTING_OPINIONS;
			}

			IR_message_t* msg = kilob_message_send();
			if(msg == NULL) break;

			msg->type = OPINION_MSG_TYPE;
			msg->data[0] = kilo_uid;
			msg->data[1] = opinion;

			break;

		/**
		 * Determine the timer value and observed quality based on the site.
		 */
		case RANDOM_WALK_SITE_INIT:
			state = RANDOM_WALK_SITE;
			quality = 0.0;
			num_quality = 0;

			kilob_random_walk_init(RW_SITE_FORWARD_MU, RW_SITE_FORWARD_SIGMA,
				RW_ROTATION_CONST, RW_ROTATION_SPAN);

			if(target_site == 0) {
				timer = kilo_ticks + (uint32_t)g_ran_exponential(SITE_A_EXPLORE_RAND_PARAM) * TICKS_PER_SEC;
			}
			else {
				timer = kilo_ticks + (uint32_t)g_ran_exponential(SITE_B_EXPLORE_RAND_PARAM) * TICKS_PER_SEC;
			}

			break;

		/*
		 * Here we observe the timer and if the timer fires, we start moving
		 * back to the nest.
		 */
		case RANDOM_WALK_SITE:
			if(kilo_ticks > timer) {
				state = MOVING_TO_NEST_INIT;
			}

			break;

		/*
		 * In this state the agent is collecting opinions until the opinion
		 * buffer fills up. This is a safeguard for the agent to always have
		 * enough opinions. Only relevant in the beginning of the experiment.
		 */
		case COLLECTING_OPINIONS:
			if(kilob_cdms_count_collected_opinions() == MAJORITY_RULE_GROUP_SIZE-1) {
				opinion = kilob_cdms_majority_rule(opinion);

				tracking_data.byte[0] = encode_opinion(kilo_uid, opinion);
				for(i=0; i<MAJORITY_RULE_GROUP_SIZE-1; i++) {
					op = kilob_cdms_collected_opinion_info(i);
					tracking_data.byte[i+1] = encode_opinion(op.kilo_id, op.opinion);
				}

				state = MOVING_TO_SITE_INIT;
			}
			break;

		default:
			break;
	}

	//call this here to have the data gatehered above in the buffer
	tracking_data.byte[TRACKING_MSG_USER_DATA-1] = state;
	kilob_tracking(&tracking_data);

	// when moving somewhere we always have active gradient
	if(kilob_gradient_has_active()) {
		//if a gradient is active we track it
		kilob_gradient_tracking();
	}
	else {
		// otherwise we perform random walk
		kilob_random_walk();
	}
}

int main() {
    kilo_init();

    utils_init();
    utils_message_rx = ir_rx;

    uint8_t i;
	
	for(i = 0; i < TRACKING_MSG_USER_DATA; i++) {
		tracking_data.byte[i] = 0;
	}

    kilo_start(setup, loop);

    return 0;
}
