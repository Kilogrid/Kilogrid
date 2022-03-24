/** @file ModuleMain.c */

#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "kilogrid.h"
#define CUSTOMIZABLE_CELL_STATES_T
#include "module.h"
#include "moduleLED.h"
#include "CAN.h"
#include "moduleIR.h"

#include "../Communication.h"


#define TRANSMISSION_INTERVAL  0.50  // [sec]


/*cell state */
volatile uint8_t c_states[N_CELLS];
volatile uint8_t c_wall_flags[N_CELLS];

volatile float next_transmission_time;
IR_message_t IR_msg_tx[N_CELLS];


/* Functions */
static void set_led(cell_num_t, uint8_t);
static void reset_leds();
static void advertise_cell_state();
static float get_current_sec();


/* Tracking data */
/*void IR_rx( IR_message_t* m, cell_num_t c, distance_measurement_t* d, uint8_t CRC_error)
{
    if(CRC_error) return;

    if(m->type == MESSAGE_FOR_TRACKING) {
        CAN_message_t* tracking_CAN_message = next_CAN_message();
        if(tracking_CAN_message != NULL) {
            serialize_tracking_message(tracking_CAN_message, c, (tracking_user_data_t*)m->data);
        }
    }
}*/

void IR_rx(IR_message_t *m, cell_num_t c, distance_measurement_t *d, uint8_t CRC_error) {
	if(!CRC_error && m->type == TRACKING){
		CAN_message_t* msg = next_CAN_message();
		if(msg != NULL) { // if the buffer is not full
			serialize_tracking_message(msg, c, m->data);
		}
	}	
}


#define min(a, b)  ((a) < (b) ? (a) : (b))
void setup()
{
    for(int i = 0; i < N_CELLS; ++i) {
      c_wall_flags[i] = 0;
      c_states[i] = 0x05;
    }

    for(int i = 0; i < min(configuration_size, N_CELLS); ++i) {
        c_wall_flags[i] = (configuration[i] & 0x10) > 0 ? 1 : 0;
        c_states[i] = configuration[i] & 0x0F;
    }

    for(int i = 0; i < N_CELLS; ++i) {
        IR_msg_tx[i].type                      = MESSAGE_ADVERTISE_CELL_STATE;
        IR_msg_tx[i].data[INDEX_CELL_STATE]    = c_states[i];
        IR_msg_tx[i].data[INDEX_WALL_FLAG]     = c_wall_flags[i];
    }
    next_transmission_time = 0;
}


void loop()
{
    float current_sec = get_current_sec();
    reset_leds();

    if(next_transmission_time <= current_sec) {
        advertise_cell_state();
        next_transmission_time = current_sec + TRANSMISSION_INTERVAL;
    }
}


int main()
{
    module_init();

    module_IR_message_rx = IR_rx;

    module_start(setup, loop);

    return 0;
}


static void set_led(cell_num_t id, uint8_t state)
{
#ifdef DISABLE_LED_ON_WALL
    // Wall check
    if(c_wall_flags[id]) {
        set_LED(id, LED_OFF);
        return;
    }
#endif

    color_t color;
    switch(state) {
        case 0:
            color = RED;
            break;
        case 1:
            color = BLUE;
            break;
        case 2:
            color = YELLOW;
            break;
        case 3:
            color = WHITE;
            break;
        case 4:
            color = GREEN;
            break;
        default:
            color = MAGENTA;
    }
    set_LED(id, color);
}


static inline void reset_leds()
{
    for(int i = 0; i < N_CELLS; ++i) {
        set_led(i, c_states[i]);
    }
}


static inline void advertise_cell_state()
{
    for(int i = 0; i < N_CELLS; ++i) {
        set_IR_message(&IR_msg_tx[i], i);
    }
}


static inline float get_current_sec()
{
    return module_ticks / (float)TICKS_PER_SEC;
}
