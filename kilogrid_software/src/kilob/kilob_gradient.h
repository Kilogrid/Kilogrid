#ifndef GRADIENT_H
#define GRADIENT_H

#include <stdint.h>
#include "message.h"
#include "utils.h"
#include "kilolib.h"

//#define TRACKING_LED_DEBUG

void kilob_gradient_tracking();

uint8_t kilob_gradient_has_active();

void kilob_gradient_tracking_init(double _check_period, double _turn_small_mu,
	double _turn_small_sigma, double _turn_large_mu, double _turn_large_sigma,
	double  _stuck_timeout);

void kilob_gradient_tracking_config_add(uint8_t gradient_idx,
	uint8_t (*direction)(uint8_t, uint8_t), void (*on_escape)(), uint8_t can_stuck);

void kilob_gradient_tracking_reset();

uint8_t kilob_gradient_tracking_decreasing(uint8_t gradient_id, uint8_t newval);

uint8_t kilob_gradient_tracking_increasing(uint8_t gradient_id, uint8_t newval);

void kilob_gradient_tracking_message_received(IR_message_t *m, distance_measurement_t *d);

#endif