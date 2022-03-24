/** @file ModuleMain.c */

/**** INCLUDES ****/

#include "debug.h"
#include <avr/io.h>
#include <avr/wdt.h> 
#include <util/delay.h>     // delay macros
#include <avr/interrupt.h>

#include "CAN.h"
#include "moduleIR.h"
#include "module.h"
#include "moduleLED.h"
#include "decision_making_common_defines.h"
#include "kilob_common.h"
#include "kilo_rand_lib.h"

/**** VARIABLES ****/
volatile uint8_t gradient_site_a[4] = {0,0,0,0};
volatile uint8_t gradient_site_b[4] = {0,0,0,0};
volatile uint8_t gradient_nest[4] = {0,0,0,0};
volatile uint8_t gradient_wall[4] = {0,0,0,0};
volatile uint8_t type = 0;
uint32_t quality_timer = 0;
uint32_t quality_timeout = MS_TO_TICKS(1000);
uint8_t green[3] = {0x0B, 0x47, 0x29};
uint8_t purple[3] = {0x68, 0x4C, 0x75};
//uint8_t purple[3] = {0x29, 0x1e, 0x2e};
uint8_t white[3] = {0xFF, 0xFF, 0xFF};

uint8_t ir_initialized = 0;
IR_message_t IR_msg_tx[4];

/**** USER DEFINED FUNCTION CALLBACKS ****/

void IR_rx(IR_message_t *m, cell_num_t c, distance_measurement_t *d, uint8_t CRC_error) {
	if(!CRC_error && m->type == TRACKING){
		CAN_message_t* msg = next_CAN_message();
		if(msg != NULL) { // if the buffer is not full
			serialize_tracking_message(msg, c, m->data);
		}
	}	
}

inline double generate_quality(double mu, double sigma) {
	double quality = g_ran_gaussian(mu, sigma);

	while(quality < 0 || quality > 2.0) {
		quality = g_ran_gaussian(mu, sigma);
	}

	return quality;
}

/**
 * @brief Trying to reference rand_soft(void) function.
 */
void setup() {
	if(configuration[0] == GRADIENT_ID_TO_SITE_A) {
		gradient_site_a[0] = configuration[1];
		gradient_site_a[1] = configuration[2];
		gradient_site_a[2] = configuration[3];
		gradient_site_a[3] = configuration[4];
	}

	if(configuration[0] == GRADIENT_ID_TO_SITE_B) {
		gradient_site_b[0] = configuration[1];
		gradient_site_b[1] = configuration[2];
		gradient_site_b[2] = configuration[3];
		gradient_site_b[3] = configuration[4];
	}

	if(configuration[0] == GRADIENT_ID_TO_NEST) {
		gradient_nest[0] = configuration[1];
		gradient_nest[1] = configuration[2];
		gradient_nest[2] = configuration[3];
		gradient_nest[3] = configuration[4];
	}

	if(configuration[0] == GRADIENT_ID_WALL) {
		gradient_wall[0] = configuration[1];
		gradient_wall[1] = configuration[2];
		gradient_wall[2] = configuration[3];
		gradient_wall[3] = configuration[4];
	}

	if(configuration[0] == 0x05) {
		type = configuration[1];
	}
	
	ir_initialized = 0;
	g_ran_set_seed_uint32(0);
}

void loop() {
	uint8_t i;
	double orig_quality = 0;

	switch(type) {
	case 0x01:
			//set_all_LEDs_with_brightness(MAGENTA, LOW);
			set_all_LEDs_hex(purple);
			break;

	case 0x02:
			//set_all_LEDs_with_brightness(WHITE, LOW);
			set_all_LEDs_hex(white);
			break;

	case 0x03:
			//set_all_LEDs_with_brightness(CYAN, MEDIUM);
			set_all_LEDs_hex(green);
			break;
	}
	

	if(module_ticks > quality_timer) {
		for(i=0; i<4; i++) {
			quality_timer = module_ticks + quality_timeout;
			
			if(type == AREA_TYPE_SITE_B) {
				orig_quality = generate_quality(QUALITY_MU_SITE_B, QUALITY_SIGMA_SITE_B);
			}
			else if(type == AREA_TYPE_SITE_A) {
				orig_quality = generate_quality(QUALITY_MU_SITE_A, QUALITY_SIGMA_SITE_A);
			}
			else {
				orig_quality = 0.0;
			}

			IR_msg_tx[i].data[5] = (uint8_t)((255.0 * orig_quality) / 2);
			set_IR_message(&IR_msg_tx[i], i);
		}
		
	}

	if(!ir_initialized) {
		ir_initialized = 1;

		for(i=0; i<4; i++) {
			IR_msg_tx[i].type = GRADIENT_MSG_TYPE;
			IR_msg_tx[i].data[0] = gradient_site_a[i];
			IR_msg_tx[i].data[1] = gradient_site_b[i];
			IR_msg_tx[i].data[2] = gradient_nest[i];
			IR_msg_tx[i].data[3] = gradient_wall[i];
			IR_msg_tx[i].data[4] = type;
			set_IR_message(&IR_msg_tx[i], i);
		}
	}
}

/**** MAIN ****/

int main() {
    module_init();

    module_IR_message_rx = IR_rx;

    module_start(setup, loop);
	
    return 0;
}
