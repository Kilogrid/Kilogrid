#include "kilob_wall_avoidance.h"
#include "kilo_rand_lib.h"
#include "ringbuffer.h"
#include "utils.h"

typedef enum{
	KILOB_WALL_AVOIDANCE_IDLE = 0x00,
	KILOB_WALL_AVOIDANCE_INIT,
	KILOB_WALL_AVOIDANCE_ROTATION,
	KILOB_WALL_AVOIDANCE_ESCAPE
} kilob_wall_avoidance_state_t;

/**
 * @brief Buffer used to store the distance readings when receiving messages
 * from wall subcells.
 */
RB_create(wall_readings_buffer, distance_measurement_t, WALL_READINGS_BUFFER_SIZE);

/**
 * @brief
 */
static const uint32_t WA_WALL_DETECTED_TIMEOUT_SEC = 1;

/**
 * @brief Timeout to leave current rotation state and rotate in the other
 * direction. It is used to avoid being stuck in the WA rotation state
 * when blocked against the wall.
 */
static const uint32_t WA_ROTATION_TIMEOUT_MS 	= 7500;

/** 
 * @brief Timer used by the wall avoidance behavior to confirm that there are
 * no walls near the kilobot.
 */
static uint32_t 		kilob_wa_wall_detected_timeout	= 0;

/**
 * @brief Timer used by the wall avoidance behavior to detect the problem where
 * a kilobot is stuck next to a wall or in a corner for a long time.
 */
//uint32_t 		kilob_wa_stuck_timeout			= 0;

/**
 * @brief State variable used by the wall avoidance behavior to keep track if
 *there were any walls detected in the last WA_WALL_DETECTED_TIMEOUT milliseconds.
 */
static uint8_t  		kilob_wa_wall_detected 			= 0;

/** 
 * @brief The direction in which the robot will turn when encountering a wall.
 * It is random with 1/2 probability. The direction is kept through the wall
 * avoidance up to a point: if the kilob_wa_stuck_timeout is reached since the first
 * encounter of a wall, then the turning direction will be reversed.
 */
static direction_t 	kilob_wa_direction 				= UNDEFINED;

static uint32_t kilob_wa_rotation_timer				= 0;

/**
 * @brief Flag that indicates that the buffer has been read and emptied, and
 * that the new value for the readings has just been computed.
 */
static volatile uint8_t has_new_wall_reading 			= 0;

/**
 * @brief This variable will store the sum of the elements of the wall readings
 * buffer, as an estimation of the proximity of (= distance to) walls.
 */
static volatile uint32_t current_wall_reading 			= 0; 

static kilob_wall_avoidance_state_t behavior_state = KILOB_WALL_AVOIDANCE_IDLE;

/**
 * @brief Previous wall reading.
 */
volatile uint32_t previous_wall_reading = 0;

static uint8_t initialized = 0;

void kilob_wall_avoidance_init() {
	if(!initialized) {
		initialized = 1;

		RB_init(wall_readings_buffer);
	}
}

/**
 *  @brief Computes a new value to estimate the proximity of walls (used internally only)
 */
static inline void compute_current_wall_reading(){
	distance_measurement_t buffer_front; // temp variable
	previous_wall_reading = current_wall_reading; // saving current value
	current_wall_reading = 0;	
	
	// compute new value of wall reading (average)
	for(uint8_t i = 0; i < WALL_READINGS_BUFFER_SIZE; i++){
		buffer_front = RB_front(wall_readings_buffer);
		RB_popfront(wall_readings_buffer); // here we do not care about the orders in which the elements of the buffer arrived. We use the buffer as an array of elements, which already has pointers to the head and size calculated.
		
		#ifdef WALL_READING_MEAN
		// sum of the elements of the buffer (= average) as an estimator of the distance to the object
		current_wall_reading += buffer_front.high_gain; 
		#elif defined(WALL_READING_MAX)
		// save current maximum in the current_wall_reading variable
		if(buffer_front.high_gain > current_wall_reading){
			current_wall_reading = buffer_front.high_gain;
		}
		#endif
	}
	
	// empty buffer (to be sure)
	RB_init(wall_readings_buffer);	
	
	has_new_wall_reading = 1; // signals that the new value is ready to be read
}

/**
 * @brief Called when a wall is detected. Sets the kilob_wa_wall_detected state variable and
 * refreshes the kilob_wa_wall_detected_timeout.
 * 
 */
inline void kilob_on_wall_detected(distance_measurement_t *wall_dist, IR_message_t *wall_msg) {
	
	if(!kilob_wa_wall_detected) {
		kilob_wa_wall_detected = 1;
	}

	kilob_wa_wall_detected_timeout = kilo_ticks + SEC_TO_TICKS(WA_WALL_DETECTED_TIMEOUT_SEC);
	
	// store wall reading
	RB_pushback(wall_readings_buffer);
	RB_back(wall_readings_buffer) = *wall_dist; // store current reading in the back of the buffer
	
	if(RB_full(wall_readings_buffer)){
				
		// save current value and compute the new one
		compute_current_wall_reading();
	}
	
}

/**
 * @brief Implements wall avoidance for the kilobot. Should be called from the main loop() function.
 *
 * The function first checks if any wall was detected in the last WA_WALL_DETECTED_TIMEOUT milliseconds.
 * If not, it sets the kilob_wa_wall_detected state variable to false and wall avoidance is finished. The details of the states
 * are as follows:
 *
 * - KILOB_WALL_AVOIDANCE_INIT and any other state not detailed here:
 *		This is the entry state, it initializes a timeout and a direction to turn the kilobot.
 *		The next state is KILOB_WALL_AVOIDANCE_ROTATION.
 *
 * - KILOB_WALL_AVOIDANCE_ROTATION:
 *		The kilobot rotates in this state until the timeout is reached. It sets the next timeout for escaping
 *		from the wall, starts the movement straight and goes to the state KILOB_WALL_AVOIDANCE_ESCAPE.
 *
 * - KILOB_WALL_AVOIDANCE_ESCAPE:
 *		Waits for the timeout and goes back to the KILOB_WALL_AVOIDANCE_INIT for reinitalization.
 *
 * @return true if the wall avoidance is still running, false if the wall was avoided.
 */
uint8_t kilob_wall_avoidance() {
	//uint32_t tmp = 0;
	double tmp_uniform = 0.0;

	if(kilo_ticks > kilob_wa_wall_detected_timeout) {
		kilob_wa_wall_detected = 0;
		kilob_wa_direction = UNDEFINED;
		behavior_state = KILOB_WALL_AVOIDANCE_IDLE;
	}

	if(!kilob_wa_wall_detected){
		return 0; // exit wall avoidance state
	}

	switch(behavior_state) {
		case KILOB_WALL_AVOIDANCE_ROTATION:
			
			if(kilo_ticks > kilob_wa_rotation_timer){
				// restart timer 
				kilob_wa_rotation_timer = kilo_ticks + MS_TO_TICKS(WA_ROTATION_TIMEOUT_MS);
				
				// toggle direction
				if(kilob_wa_direction == LEFT){
					kilob_wa_direction = RIGHT;
					turn_right();
				}
				else{
					kilob_wa_direction = LEFT;
					turn_left();
				}
				
				// invalidate proximity buffer
				RB_init(wall_readings_buffer);
				current_wall_reading = 0;
				previous_wall_reading = 0;
				
				#ifdef ENABLE_LIGHTS_RW_WA
				set_color(RGB(1,0,1)); // indicate toggling
				#endif
			}
			
			if(has_new_wall_reading){
				has_new_wall_reading = 0; // acknowledge that we read the new wall reading
				
				// if we got away from the wall: escape
				if(previous_wall_reading > current_wall_reading){
					move_straight();
					#ifdef ENABLE_LIGHTS_RW_WA
					set_color(RGB(1,1,1)); // white
					#endif
					behavior_state = KILOB_WALL_AVOIDANCE_ESCAPE;
				}		
				// else: keep rotating
			}

			break;

		case KILOB_WALL_AVOIDANCE_ESCAPE:
			
			if(has_new_wall_reading){
				has_new_wall_reading = 0; // acknowledge that we read the new wall reading
				
				// if we came closer to a wall: go back to rotate state
				if(previous_wall_reading < current_wall_reading){
					behavior_state = KILOB_WALL_AVOIDANCE_INIT;
				}		
				// else: a wall timeout will induce an exit of this state to the random walk state
			}
			break;

		case KILOB_WALL_AVOIDANCE_INIT:
		default:
			
			tmp_uniform = g_ran_uniform();
			
			if(tmp_uniform > 0.5) {
				kilob_wa_direction = LEFT;
			}
			else {
				kilob_wa_direction = RIGHT;
			}

			if(kilob_wa_direction == LEFT) {
				turn_left();
			}
			else {
				turn_right();
			}
			
			// reinit proximity estimators
			current_wall_reading = 0;
			previous_wall_reading = 0;
			RB_init(wall_readings_buffer); 
			
			// initialize rotation timeout timer
			kilob_wa_rotation_timer = kilo_ticks + MS_TO_TICKS(WA_ROTATION_TIMEOUT_MS);

			#ifdef ENABLE_LIGHTS_RW_WA
			set_color(RGB(1,0,0)); 
			#endif
			behavior_state = KILOB_WALL_AVOIDANCE_ROTATION;

			break;
	}

	return kilob_wa_wall_detected;
}

/**
 * @brief Resets the state to KILOB_WALL_AVOIDANCE_INIT. The rest of the initialization is done in
 * kilob_wall_avoidance().
 */
inline void kilob_wall_avoidance_reset() {
	kilob_wa_direction = UNDEFINED;
	behavior_state = KILOB_WALL_AVOIDANCE_INIT;
}
