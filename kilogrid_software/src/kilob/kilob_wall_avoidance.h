#ifndef __KILOB_WALL_AVOIDANCE_H__
#define __KILOB_WALL_AVOIDANCE_H__

#include "kilolib.h"
#include "message.h"

#define WALL_READING_MEAN
#define WALL_DEPTH				2 // length of the chain of propagation of wall beacons (CELL -> KILOBOT -> KILOBOT -> ...)

/**
 * @brief Size of the buffer to store LGAIN values of the signal sent by walls
 * during wall avoidance. This buffer is used to compute an average power for
 * the wall readings.
 */
#define WALL_READINGS_BUFFER_SIZE 4

void kilob_wall_avoidance_init();
void kilob_on_wall_detected(distance_measurement_t*, IR_message_t*);
uint8_t kilob_wall_avoidance();
void kilob_wall_avoidance_reset();

#endif