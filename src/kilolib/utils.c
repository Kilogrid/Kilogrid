#include <stdio.h>
#include <avr/io.h>         // io port and addresses
#include <util/delay.h>     // delay macros

#include "kilolib.h"
#include "utils.h"
#include "macros.h"

/**** Variables ****/

direction_t direction = UNDEFINED;
uint8_t motors_enabled = 0;

inline void try_enable_motors() {
	if(!motors_enabled) {
		motors_enabled = 1;
		motors_on();
	}
}

void my_dummy(IR_message_t *m, distance_measurement_t *d) {}
message_rx_t utils_message_rx = my_dummy;

void utils_rx(IR_message_t *m, distance_measurement_t *d) {
	switch(m->type) {
		case GO_STRAIGHT:
			try_enable_motors();
			move_straight();
			break;

		case TURN_LEFT:
			try_enable_motors();
			turn_left();
			break;

		case TURN_RIGHT:
			try_enable_motors();
			turn_right();
			break;

		default:
			utils_message_rx(m, d);
	}
}

inline void utils_init() {
	kilo_message_rx = utils_rx;
}

/**** Functions ****/

inline void safe_set_motors(uint8_t ccw, uint8_t cw) {
	if(ccw > MOTOR_LEFT_MAX) {
		ccw = MOTOR_LEFT_MAX;
	}

	if(cw > MOTOR_RIGHT_MAX) {
		cw = MOTOR_RIGHT_MAX;
	}

    OCR2A = ccw;
    OCR2B = cw;
}

inline void init_motors() {
	direction = UNDEFINED;
	stop();
}

inline void turn_left() {
	if(direction != LEFT) {
		direction = LEFT;
		set_motors(255, 255);
    	delay(20);
		safe_set_motors(kilo_turn_left, 0);
	}
}

inline void turn_right() {
	if(direction != RIGHT) {
		direction = RIGHT;
		set_motors(255, 255);
    	delay(20);
		safe_set_motors(0, kilo_turn_right);
	}
}

inline void move_straight() {
	if(direction != STRAIGHT) {
		//set_motors(0, 0);
		//delay(1000);
		direction = STRAIGHT;
		set_motors(255, 255);
    	delay(20);
		safe_set_motors(kilo_straight_left, kilo_straight_right);
	}
}

inline void stop() {
	direction = UNDEFINED;
	safe_set_motors(0,0);
}

inline direction_t get_direction(){
	return direction;
}