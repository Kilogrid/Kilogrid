/** @file utils.h */

#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>
#include "kilolib.h"

#define MOTOR_LEFT_MAX					100
#define MOTOR_RIGHT_MAX					100
#define MS_PER_TICK  					32

/** @brief Convert tick time into milliseconds.
 */
#define TICKS_TO_MS(a) 					(a * MS_PER_TICK)
#define MS_TO_TICKS(a) 					(a / MS_PER_TICK)
#define SEC_TO_TICKS(a)					(a * 32)

typedef enum {
	UNDEFINED = 0x00, 
	LEFT, 
	RIGHT, 
	STRAIGHT
} direction_t;

extern message_rx_t utils_message_rx;

void utils_init();

/**
 * @brief Safely set the power of each motor. 
 *
 * The power received by the left and right motor is controlled using
 * hardware pulse-width-modulation (PWM) and can be set using this
 * function. Note that if the power values exceeds the MOTOR_LEFT-RIGHT_MAX values, these values are taken as a default.
 *
 * The parameter @p left and @p right are 8-bit unsigned integers (0 to
 * MOTOR_LEFT-RIGHT_MAX) that control the duty-cycle of the PWM signal from 0% to 100% * (MOTOR_LEFT-RIGHT_MAX / 255).
 * In other words, setting a motor to 0% duty-cycle equates to running
 * off the motor, and setting a motor to 50% duty-cycle equates to
 * running the motor at half the full power. For the most part, motors should
 * only be set to the calibrated values `kilo_turn_left`,
 * `kilo_turn_right`, `kilo_straight_left` and `kilo_straight_right`.
 *
 * When a motor transitions from being off (0% duty cycle) to being on
 * (> 10% duty cycle) it must first be turned on at full-speed for 15ms
 * to overcome the effects of static friction. 
 *
 * @see kilo_turn_left
 * @see kilo_turn_right
 * @see kilo_straight_left
 * @see kilo_straight_right
 * @see MOTOR_LEFT_MAX
 * @see MOTOR_RIGHT_MAX
 *
 * @code
 * // turn motors off
 * set_motors(0, 0);
 * // spin up left motor for 15ms
 * set_motors(255, 0);
 * delay(15);
 * // turn the kilobot left for 2 seconds
 * set_motors(kilo_turn_left, 0);
 * delay(2000);
 * // go straight for 2 seconds
 * set_motors(kilo_straight_left, kilo_straight_right);
 * delay(2000);
 * @endcode
 *
 * @param left 8-bit integer to control left motor duty-cycle.
 * @param right 8-bit integer to control right motor duty-cycle.
 *
 * @warning It is important that in a 2 second interval no motor is at
 * 100% duty-cycle (255) for more than 50ms at a time, since this might
 * cause the motors to fail permanently. It is recommended that during
 * regular operation the motors are never run to more than a 35%
 * duty-cycle (90).
 *
 * @warning This is a modified version of the
 * set_motors(uint8_t left, uint8_t right) function to prevent
 * damaging the motors. The maximum duty-cycle is capped by macros
 * MOTOR_LEFT_MAX and MOTOR_RIGHT_MAX.
 */
void safe_set_motors(uint8_t left, uint8_t right);

/**
 * @brief Must be called to read calibration values from the eeprom.
 */
void init_motors();

/**
 * @brief Use this function to turn left.
 */
void turn_left();

/**
 * @brief Use this function to turn right.
 */
void turn_right();

/**
 * @brief Use this function to move straight.
 */
void move_straight();

/**
 * @brief Use this function to stop the robot.
 */
void stop();

/**
 * @brief Use this function to get the current direction of the robot.
 */
direction_t get_direction();

#endif