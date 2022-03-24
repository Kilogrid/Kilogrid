#include "kilob_gradient.h"
#include "kilolib.h"
#include "kilo_rand_lib.h"
#include <math.h>

#define GRADIENT_CONFIG_LENGTH 4

static uint32_t check_period;
static uint32_t turn_small_mu;
static uint32_t turn_small_sigma;
static uint32_t turn_large_mu;
static uint32_t turn_large_sigma;
static uint32_t stuck_timeout;

static const uint8_t SMALL_VALUE = 0;
static const uint8_t BIG_VALUE = 0xFF;

typedef enum{
	GRADIENT_TRACKING_TRACK = 0x00,
	GRADIENT_TRACKING_TURN
} kilob_gradient_tracking_state_t;

typedef struct {
	uint8_t idx; /// index of the gradient
	uint8_t (*direction)(uint8_t, uint8_t); /// direction to follow on gradient
	void (*on_escape)(); /// callback to call when we escaped from the gradient
	uint8_t past_maximum; /// maximum value in the past
	uint8_t past_minimum; /// minimum value in the past
	uint8_t current_maximum; /// currently observed maximum
	uint8_t current_minimum; /// currently observed minimum
	uint8_t escaped; /// true if the gradient is escaped
	uint8_t has_data; /// true if new data was received since last check
	uint8_t can_stuck; /// true if the gradient is a wall type, where the robots can get stuck
	uint32_t detected_at; /// timestamp of the gradient's detection
} kilob_gradient_tracing_config_t;

static uint32_t check_timer = 0;
static uint8_t config_items = 0;

static kilob_gradient_tracing_config_t config[GRADIENT_CONFIG_LENGTH];

static kilob_gradient_tracking_state_t state;
static direction_t turning_dir = UNDEFINED;

/**
 * @brief Private function, called from places, where refreshing the
 * state variables is needed. Overrides past values with current ones
 * and sets current ones to small and big values for maximum and
 * minimum respectively.
 */
void kilob_gradient_update_values() {
	uint8_t i;

	for(i=0; i<config_items; i++) {
		config[i].past_minimum = config[i].current_minimum;
		config[i].past_maximum = config[i].current_maximum;
		config[i].current_minimum = BIG_VALUE;
		config[i].current_maximum = SMALL_VALUE;
	}
}

/**
 * @brief Private function, initiates evasion, sets up state and
 * timer needed. If the direction is UNDEFINED, it chooses one
 * randomly.
 */
void kilob_gradient_evade(uint32_t turn_mu, uint32_t turn_sigma) {
	// if already turning, skip this
	if(state == GRADIENT_TRACKING_TURN) {
		return;
	}

	state = GRADIENT_TRACKING_TURN;
	if(turning_dir == UNDEFINED) {
		if(g_ran_uniform() < 0.5) {
			turning_dir = RIGHT;
		}
		else {
			turning_dir = LEFT;
		}
	}

	if(turning_dir == LEFT) {
		turn_left();
	}
	else {
		turn_right();
	}
	
	check_timer = kilo_ticks + (uint32_t)fabs(g_ran_gaussian(turn_mu, turn_sigma));
}

uint8_t kilob_gradient_has_active() {
	uint8_t i;

	for(i=0; i<config_items; i++) {
		if(!config[i].escaped) {
			return 1;
		}
	}

	return 0;
}

/**
 * @brief This is the function, that the user is calling from the loop()
 * function. Its keeps track of all the gradients and manages state.
 */
void kilob_gradient_tracking() {
	uint8_t action_needed = 0;
	uint8_t all_escaped = 0;
	uint8_t i;

	// If the time is not up, we return
	if(kilo_ticks < check_timer) {
		return;
	}

	#ifdef TRACKING_LED_DEBUG
	set_color(RGB(0,0,0));
	#endif

	/*
	 * If the last state was turning, we update the minimum and maximum
	 * values, change the state and set up the timer, then return.
	 */
	if(state == GRADIENT_TRACKING_TURN) {
		kilob_gradient_update_values();
		move_straight();
		state = GRADIENT_TRACKING_TRACK;
		check_timer = kilo_ticks + check_period;
		return;
	}

	/*
	 * Here we are checking if the minimum and maximum values for a gradient
	 * is the same. That means, that we are moving perpendicular to the
	 * gradient.
	 */
	for(i=0; i<config_items; i++) {
		if(!config[i].has_data) {
			#ifdef TRACKING_LED_DEBUG
			set_color(RGB(1,1,0));
			#endif
			continue;
		}

		/*
		 * If the values are different than 0 but the same since we last checked
		 * we indicate that action is needed.
		 */
		if( config[i].past_minimum != 0 &&
			config[i].past_maximum != 0 &&
			config[i].past_minimum == config[i].current_minimum &&
			config[i].past_maximum == config[i].current_maximum)
		{
			action_needed = 1;

			#ifdef TRACKING_LED_DEBUG
			set_color(RGB(0,0,1));
			#endif
		}

		/*
		 * If the maximum value is 0, it means that we escaped that gradient.
		 * We reset the detected_at timestamp and call the callback associated
		 * with this gradient.
		 */
		if(config[i].current_maximum == 0) {
			config[i].escaped = 1;
			config[i].detected_at = 0;
			config[i].on_escape();
		}

		/*
		 * Stuck behavior is implemented here. If a gradient can be stuck we
		 * first check if it was detected (value > 0) and then check
		 * for the stuck timeout.
		 *
		 * If we change the turning direction, we update the timer too.
		 */
		if(config[i].can_stuck == 1 &&
		   config[i].detected_at > 0 &&
		   kilo_ticks - config[i].detected_at > stuck_timeout)
		{
			turning_dir = (turning_dir == LEFT) ? RIGHT : LEFT;
			config[i].detected_at = kilo_ticks;
		}

		config[i].has_data = 0;
	}

	// Count the number or gradients escaped.
	for(i=0; i<config_items; i++) {
		if(config[i].escaped) all_escaped++;
	}

	/*
	 * If all gradients were escaped, the next gradient encounter will result in a
	 * random turning direction. This is to prevent choosing the turning direction
	 * randomly all the time. That behavior could result in traversing the
	 * gradient in the wrong direction.
	 */
	if(all_escaped == config_items) {
		turning_dir = UNDEFINED;

		#ifdef TRACKING_LED_DEBUG
		set_color(RGB(1,1,1));
		#endif
	}

	// Update minimum and maximum values, reset current values
	kilob_gradient_update_values();

	// If action is needed we turn a small amount
	if(action_needed) {
		kilob_gradient_evade(turn_small_mu, turn_small_sigma);
	}
	else {
		check_timer = kilo_ticks + check_period;
	}
}

/**
 * @brief Initializes gradient tracking behavior.
 */
void kilob_gradient_tracking_init(double _check_period, double _turn_small_mu,
	double _turn_small_sigma, double _turn_large_mu, double _turn_large_sigma,
	double _stuck_timeout) {

	check_period = _check_period * TICKS_PER_SEC;
	turn_small_mu = _turn_small_mu * TICKS_PER_SEC;
	turn_small_sigma = _turn_small_sigma * TICKS_PER_SEC;
	turn_large_mu = _turn_large_mu * TICKS_PER_SEC;
	turn_large_sigma = _turn_large_sigma * TICKS_PER_SEC;
	stuck_timeout = _stuck_timeout * TICKS_PER_SEC;

	state = GRADIENT_TRACKING_TRACK;
	config_items = 0;
}

/**
 * @brief 
 */
void kilob_gradient_tracking_config_add(uint8_t gradient_idx,
	uint8_t (*direction)(uint8_t, uint8_t), void (*on_escape)(), uint8_t can_stuck) {
	
	if(config_items == GRADIENT_CONFIG_LENGTH) {
		return;
	}

	config[config_items].idx = gradient_idx;
	config[config_items].direction = direction;
	config[config_items].on_escape = on_escape;
	config[config_items].past_minimum = BIG_VALUE;
	config[config_items].past_maximum = SMALL_VALUE;
	config[config_items].escaped = 0;
	config[config_items].can_stuck = can_stuck;

	config_items += 1;
}

void kilob_gradient_tracking_reset() {
	check_timer = kilo_ticks + check_period;
	config_items = 0;
}

/**
 * @brief Direction tracking function, used for configuration of gradient
 * tracking.
 *
 * @return true, if action is needed for this direction.
 */
uint8_t kilob_gradient_tracking_decreasing(uint8_t gradient_id, uint8_t newval) {
	if(newval == 0) {
		return 0;
	}

	return newval > config[gradient_id].past_maximum;
}

/**
 * @brief Direction tracking function, used for configuration of gradient
 * tracking.
 *
 * @return true, if action is needed for this direction.
 */
uint8_t kilob_gradient_tracking_increasing(uint8_t gradient_id, uint8_t newval) {
	if(newval == 0) {
		return 0;
	}

	return newval < config[gradient_id].past_minimum;
}

/**
 * @brief Infrared message handler function. Parses incoming data, tracks
 * minimum and maximum values for different gradients, initiates evasion
 * if needed.
 */
void kilob_gradient_tracking_message_received(IR_message_t *m, distance_measurement_t *d) {
	uint8_t i;
	uint8_t idx, value;

	for(i=0; i<config_items; i++) {
		idx = config[i].idx;
		value = m->data[idx];

		config[i].has_data = 1;

		if(value > 0) {
			config[i].escaped = 0;
			if(config[i].detected_at == 0) {
				config[i].detected_at = kilo_ticks;
			}
		}

		if(value < config[i].current_minimum) {
			config[i].current_minimum = value;
		}

		if(value > config[i].current_maximum) {
			config[i].current_maximum = value;
		}

		if(state == GRADIENT_TRACKING_TRACK) {
			if(config[i].direction(i, value)) {

				#ifdef TRACKING_LED_DEBUG
				set_color(RGB(1,0,0));
				#endif

				kilob_gradient_evade(turn_large_mu, turn_large_sigma);
			}
		}
	}
}
