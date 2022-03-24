/** @file CellMain.c */

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

typedef enum{
	TEST_INIT = 0x00,
	TEST_LEDS_R,
	TEST_LEDS_G,
	TEST_LEDS_B,
	TEST_IR_TX,
	TEST_IR_RX
} module_testing_state_t; //cell states

#define DURATION_LED_R_TEST_TICKS 2 * TICKS_PER_SEC
#define DURATION_LED_G_TEST_TICKS 2 * TICKS_PER_SEC
#define DURATION_LED_B_TEST_TICKS 2 * TICKS_PER_SEC
#define DURATION_IR_TX_TEST_TICKS 6 * TICKS_PER_SEC
#define DURATION_IR_RX_TEST_TICKS 10 * TICKS_PER_SEC

IR_message_t IR_tx_msg;

volatile module_testing_state_t module_testing_state;	// store the current testing state of the cell
volatile uint32_t timer;
volatile uint8_t color[4] = {0,0,0,0};

/**** USER DEFINED FUNCTION CALLBACKS ****/

void IR_rx(IR_message_t *m, cell_num_t c, distance_measurement_t *d, uint8_t CRC_error) {
	if(module_testing_state == TEST_IR_RX) {
		color[c] = m->data[2];
		CAN_message_t* msg = next_CAN_message();
		if(msg != NULL)
		{
		 serialize_tracking_message(msg, c,(tracking_user_data_t*)m->data);//transmit message through CAN
		}
	}
}  //if in TEST_IR_RX state, change the color of a cell to the one received in the message from the kilobot

void IR_diffuse_color(cell_num_t c, color_t col){
	IR_tx_msg.type = 0x22; //message type, can be decided by the user
	IR_tx_msg.data[0] = col; // color in the message
	set_IR_message(&IR_tx_msg, c); // prepare the message for transmission
	set_LED(c, col); // light up the subcell with the right color
}

void IR_diffuse_color_all(color_t col){
	IR_tx_msg.type = 0x22; //message type
	IR_tx_msg.data[0] = col; // color in the message
	for(uint8_t i = 0; i < 4; ++i) {//sets the LEDs of all the cells in the module to color c and sends it as a message.
		set_IR_message(&IR_tx_msg, i); // prepare the message for transmission
		set_LED(i, col); // light up the subcell with the right color
	}
}

void IR_rx_start_receiving(cell_num_t c)//sends a message. Kilobots receiving the message will switch from a state of receiving colors from the cells toone of sending colors to the cells
{
	IR_tx_msg.type = 0x23; //message type
	set_IR_message(&IR_tx_msg, c);
}

// setup() is a loop function executed by cell lib when clicking on the Setup button in KiloGUI.
void setup(){
	module_testing_state = TEST_INIT;
	set_all_LEDs(LED_OFF);
}
//loop function, code executed when clicking on the Run button in KiloGUI
void loop() {

	switch(module_testing_state){//switches through all the testing states
		case TEST_LEDS_R:
			IR_diffuse_color_all(RED);//LEDs of all the cells are set to red

			if(module_ticks > timer) {
				timer = module_ticks + DURATION_LED_G_TEST_TICKS;
				module_testing_state = TEST_LEDS_G;
			}
			break;

		case TEST_LEDS_G:
			IR_diffuse_color_all(GREEN);//LEDs of all the cells are set to green

			if(module_ticks > timer) {
				timer = module_ticks + DURATION_LED_B_TEST_TICKS;
				module_testing_state = TEST_LEDS_B;
			}
			break;

		case TEST_LEDS_B:
			IR_diffuse_color_all(BLUE);//LEDs of all the cells are set to blue

			if(module_ticks > timer) {
				timer = module_ticks + DURATION_IR_TX_TEST_TICKS;
				module_testing_state = TEST_IR_TX;
			}
			break;

		case TEST_IR_TX://LEDs of cells in each module are set to the following colors
			IR_diffuse_color(CELL_00, BLUE);
			IR_diffuse_color(CELL_01, GREEN);
			IR_diffuse_color(CELL_02, YELLOW);
			IR_diffuse_color(CELL_03, RED);

			if(module_ticks > timer) {
				timer = module_ticks + DURATION_IR_RX_TEST_TICKS;
				module_testing_state = TEST_IR_RX;
				for(int i=0;i<4;++i)
				{
					IR_rx_start_receiving(i);
				}
				
			}
			break;

		case TEST_IR_RX:
			for(int i=0; i<4; i++) {
				set_LED(i, color[i]); //set the color of the cell to the one received from a Kilobot nearby
			}

			if(module_ticks > timer) {
				module_testing_state = TEST_INIT;
			}
			break;

		case TEST_INIT:
			timer = module_ticks + DURATION_LED_R_TEST_TICKS;
			module_testing_state = TEST_LEDS_R;

			break;
	}
}

/**** MAIN ****/

int main() {
	// initialize all peripherals of the cell
    module_init();

	module_IR_message_rx = IR_rx;
	init_IR_message(&IR_tx_msg);

    module_start(setup, loop);

    return 0;
}
