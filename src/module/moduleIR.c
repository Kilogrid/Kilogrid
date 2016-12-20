/** @file moduleIR.h */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>     // read eeprom values
#include <util/delay.h>     // delay macros

#include <utils.h>
#include <ADC.h>
#include <debug.h>
#include <message_send.h>
#include <message_crc.h>

#include "module.h"
#include "moduleIR.h"
#include "IO.h"

#ifdef DEBUG_LIGHTS_ON
#   include "moduleLED.h"   // for visual debugging
#endif


// IR RX
static IR_message_t rx_message_buffer;          ///< message being received - input buffer
static distance_measurement_t rx_dist;          ///< signal strength of message being received
static volatile uint8_t rx_busy;                ///< flag that signals if message is being received
static uint8_t rx_leadingbit;                   ///< flag that signals start bit
static uint8_t rx_leadingbyte;                  ///< flag that signals start byte
static uint8_t rx_byteindex;                    ///< index to the current byte being decoded
static uint8_t rx_bytevalue;                    ///< value of the current byte being decoded
static uint16_t mux_timer_counter;              ///< counter for Timer 2 interrupts
static uint16_t mux_timer_compare;              ///< comp[are value for the mux_timer_counter var

static volatile cell_num_t current_rx_cell;     ///< store the current cell of the RX MUX
static volatile cell_num_t current_tx_cell;     ///< store the current cell of the TX MUX

static IR_message_t IR_rx_message[N_CELLS];     ///< store the array of messages to be sent
static uint8_t CRC_error[N_CELLS];              ///< indicate whether the value of the CRC of the received message is erroneous

// pointer to the message being received - used by the ADC comparator interrupt
static uint8_t *rawmsg = (uint8_t*)&rx_message_buffer;

// IR TX
volatile uint8_t tx_mask;                       ///< used previously in the kilobot - used by the ASM message_send.S file to send an IR message
static volatile uint8_t debug_tx_flag;          ///< debug only
static volatile uint16_t module_tx_period;      ///< same as the kilo_tx_period variable in the Kilobot
static uint16_t tx_clock;                       ///< number of timer cycles we have waited
static uint16_t tx_increment;                   ///< number of timer cycles until next interrupt
static uint16_t cell_irhigh[N_CELLS][14];       ///< store calibration values to compute the distance
static uint16_t cell_irlow[N_CELLS][14];        ///< store calibration values to compute the distance

static IR_message_t IR_tx_message[N_CELLS];     // store the address of the array of messages to be sent
static uint8_t IR_enable_tx[N_CELLS];           // enable transmitter flags


/**
 * @brief Initialization of the IR communication functionalities and variables.
 * The transmission is enabled by default on the cell.
 */
void init_module_IR(void){

	uint8_t i, s; // counters

	/* Ports */

	// IR RX
	SET_AS_INPUT(IRRXCOM_D);
	SET_AS_INPUT(IRRXCOM_A);
	SET_AS_INPUT(VOLTAGE_REF);

	SET_AS_OUTPUT(IRRXADDR0);
	SET_AS_OUTPUT(IRRXADDR1);
	SET_AS_OUTPUT(IRRXADDR2);

	SELECT_IR_RECEIVER(CELL_00, HGAIN);
	SELECT_IR_TRANSMITTER(CELL_00);

#ifdef DEBUG_LIGHTS_ON
    set_LED(current_rx_cell, BLUE);
#endif

	// IR TX
	SET_AS_OUTPUT(IRTXCOM);
	SET_AS_OUTPUT(IRTXADDR0);
	SET_AS_OUTPUT(IRTXADDR1);

	/* Variables */
	rx_busy           = 0;
	rx_leadingbit     = 1;
	rx_leadingbyte    = 1;
	rx_byteindex      = 0;
	rx_bytevalue      = 0;
	current_rx_cell   = 0;
	current_tx_cell   = 0;
	mux_timer_counter = 0;

	tx_mask = (1 << IRTXCOMbit);  // TODO: remove - does not have any meaning in the module
	// CAREFUL: the message_send function relies on the tx_mask. The message_send function might return a 1 even if the message was not correctly sent!!!

	tx_clock = 0;
	tx_increment = 255;

	module_ticks = 0;

	module_tx_period = 977;  // 3906 / 4 --> we send a message every 499.968 ms / 4 on each subcell ONE AFTER THE OTHER

	// initialize received message buffer
	init_IR_message(&rx_message_buffer);

	// TODO put calibration values for all subcells in EEPROM and assign them here
	for(s = 0; s < N_CELLS; s++){

		// initialize messages arrays
		IR_enable_tx[s] = 0;
		init_IR_message(&IR_tx_message[s]);
		init_IR_message(&IR_rx_message[s]);
		CRC_error[s] = 0;

		// initialize calibration values - TODO remove?
		for (i = 0; i < 14; i++) {
			cell_irlow[s][i] = (eeprom_read_byte(EEPROM_IRLOW + i*2) << 8) | eeprom_read_byte(EEPROM_IRLOW + i*2+1);
			cell_irhigh[s][i] = (eeprom_read_byte(EEPROM_IRHIGH + i*2) << 8) | eeprom_read_byte(EEPROM_IRHIGH + i*2+1);
		}

	}

	/* Timers setup */
	tx_timer_setup();
	rx_timer_setup();
	rx_mux_timer_setup();

	/** Transmission and rx cycling enabled by default */
	tx_timer_on();
	rx_mux_timer_on();

	mux_timer_compare = 1;
}


/**
 * @brief Initialize a IR_message_t structure. Set whole message to 0.
 *
 * @param m	Message to be initialized.
 */
void init_IR_message(IR_message_t *m){
	uint8_t d;

	m->crc = 0;
	m->type = 0;
	for(d = 0; d < 9; d++) m->data[d] = 0; // initialize all data bytes
}

/**
 * @brief Set a message to be sent on subcell number "sc".
 * This function enables the transmission on one subcell (IR_enable_tx[sc] = 1), and prepares the message for transmission
 * (calculate and set crc).
 *
 * @param m		Message to be sent.
 * @param c     Identifier of the cell to send the message from.
 */
void set_IR_message(IR_message_t *m, cell_num_t c){
	IR_tx_message[c] = *m;  // copy message pointed by m into internal IR_tx_message array
	IR_tx_message[c].crc = message_crc(&IR_tx_message[c]); // compute message CRC
	IR_enable_tx[c] = 1;    // set transmission flag on subcell sc by default
}

/**
 * @brief Disable transmission on subcell "c".
 *
 * @param c     Identifier of the cell.
 */
void disable_IR_tx(cell_num_t c){
	IR_enable_tx[c] = 0; // set tx enable flag to 0
}

/**
 * @brief Timer0 interrupt.
 * Used to send messages every cell_tx_period ticks.
 */
// TODO move to cell_lib --> access to the cell_ticks variable (should be related to the Cell_lib high level file, not CellIR)
// Here's what to do:
// - Create function static inline update_IR_TX()
// - Setup TIMER0 in Cell_lib's cell_init()
// - Call function update_IR_TX() in Cell_lib TIMER0 ISR
ISR(TIMER0_COMPA_vect) {

	uint8_t s; // counter
	IR_message_t *msg; // message to be sent - TODO remove (redundant)

	tx_clock += tx_increment;
	tx_increment = 0xFF; // reset tx_increment to its default value (timer in compare mode)
	tx_timer_set_compare(tx_increment)
	module_ticks++;

	if(!rx_busy && tx_clock > module_tx_period){ // Why not change when rx is busy?, are they dependent?
		tx_clock = 0;

		current_tx_cell++; // progress to the next cell

		if(current_tx_cell == N_CELLS){
			current_tx_cell = 0; // reset counter
		}

		s = current_tx_cell;

		// check if a message needs to be sent on every subcell
		// MODIF: we do not send on each subcell inside the interrupt, but rather switch the TX subcell at each timer interrupt
		//for(s = 0; s < N_SUBCELLS; s++){
			if(IR_enable_tx[s]) {

				msg = &IR_tx_message[s];

				if (msg) { // if msg is a valid pointer - todo remove (unnecessary)

					SELECT_IR_TRANSMITTER(s); // configure mux to send on subcell s

					if (message_send(msg)){ // send message, return true if succeeded

						module_IR_message_tx_success(s);
						tx_clock = 0;
						//IR_enable_tx[s] = 0; // reset flag - do not do this (continuous sending allowed)

					}
					else{
						tx_increment = rand() & 0xFF;
						tx_timer_set_compare(tx_increment)
					} // if message tx success
				} // if message
			} // if IR_enable_tx
		// } // for every subcell

	} // if rx not busy and tx_clock overflow
}

/**
 * @brief Timer1 interrupt.
 * Timeout which is triggered if stop bit is not received.
 */
ISR(TIMER1_COMPA_vect) {
    rx_timer_off();
    rx_leadingbit = 1;
    rx_leadingbyte = 1;
    rx_busy = 0;
	SELECT_IR_RECEIVER(current_rx_cell, HGAIN);
}

/**
 * @brief Timer 2 interrupt.
 * This ISR changes the IR RX MUX address bits constantly, until a message is detected on the RX COM pin.
 * In this case, this timer is stopped by
 */
ISR(TIMER2_COMPA_vect){

	mux_timer_counter += 1;

	if(!rx_busy && mux_timer_counter >= mux_timer_compare){ // switch to another subcell only if reception is not active
		mux_timer_counter = 0;

#ifndef NO_SWITCHING // only used during tests

			current_rx_cell += 1;
			if(current_rx_cell == N_CELLS) {
				current_rx_cell = 0;  // back to cell CELL_00

#   ifdef DEBUG_LIGHTS_ON
				set_LED(current_rx_cell, BLUE);
				set_LED(CELL_03, LED_OFF);
#   endif
			}

#   ifdef DEBUG_LIGHTS_ON
			else{
				set_LED(current_rx_cell, BLUE);
				set_LED(current_rx_cell - 1, LED_OFF);
			}
#   endif

#endif

		SELECT_IR_RECEIVER(current_rx_cell, HGAIN); // // Change IR RX MUX outputs. select the subcell that is sensitive to an IR incoming message. ADC is sensitive to the HGAIN signal.

	}
}

/**
 * @brief Analog comparator trigger interrupt.
 * Triggered by incoming IR pulses (i.e. individual bits).
 * Progressively fills in the rawmsg[] array that points to the rx_message_buffer message
 * struct.
 */

ISR(ANALOG_COMP_vect) {

	cli();

    uint16_t timer = TCNT1;             // save Timer 1 state

    rx_busy = 1;                        // inhibits parts in the other interrupts

    if(rx_leadingbit) {                 // start bit received.
        rx_timer_on();                  // switch on timeout timer
        rx_bytevalue = 0;               // start at byte 0
        rx_leadingbit = 0;              // reset rx_leadingbit flag
        if (rx_leadingbyte) {
            ADC_FINISH_CONVERSION();
            rx_dist.high_gain = ADCW;   // measure the high gain value of the IR signal at the first bit of the first byte received

			// change to low gain signal until next bit arrives
			SELECT_IR_RECEIVER(current_rx_cell, LGAIN);

        }
    }
	else { // non starting bit received

        if (timer <= rx_bitcycles/2 || timer >= rx_bitcycles*9+rx_bitcycles/2) {
            rx_timer_off();
            rx_leadingbit = 1; // prepare for next start bit to arrive
            rx_leadingbyte = 1;
            rx_busy = 0;
			SELECT_IR_RECEIVER(current_rx_cell, HGAIN);

        }
		else {
            // NOTE: The following code avoids a division which takes
            // too many clock cycles and throws off the interrupt.
            const uint16_t M = ((1L<<16)+rx_bitcycles-1)/rx_bitcycles;
            uint8_t bitindex = ((uint32_t)(timer-rx_bitcycles/2)*M)>>16;
            if (bitindex <= 7) {
                rx_bytevalue |= (1<<bitindex);
            }
			else{  // stop bit of a byte received
                rx_leadingbit = 1; // prepare for next start bit to arrive
                if (rx_leadingbyte) {
                    ADC_FINISH_CONVERSION();
                    rx_dist.low_gain = ADCW; // measure the low value of the gain at the first byte of a message

					// Switch back to high gain
 					SELECT_IR_RECEIVER(current_rx_cell, HGAIN);

                    if (rx_bytevalue != 0) { // Collision detected.
                        rx_timer_off();
                        rx_leadingbyte = 1;
                        rx_busy = 0;

						//TCCR2B |= (1 << CS20) | (1 << CS21) | (1 << CS22); // restart mux timer
                    }
					else {
                        rx_leadingbyte = 0;
                        rx_byteindex = 0;
                    }
                }
				else {
                    rawmsg[rx_byteindex] = rx_bytevalue; // byte decoded --> store it
                    rx_byteindex++; // prepare for next byte to come
                    if (rx_byteindex == sizeof(IR_message_t)) { // if all bytes have been received
                        rx_timer_off();
                        rx_leadingbyte = 1;
                        rx_busy = 0; // release rx_busy flag

						CRC_error[current_rx_cell] = ( rx_message_buffer.crc != message_crc(&rx_message_buffer) ); // check received message CRC and compare with the calculated one
						IR_rx_message[current_rx_cell] = rx_message_buffer; // store message buffer into internal array
						module_IR_message_rx(&IR_rx_message[current_rx_cell], current_rx_cell, &rx_dist, CRC_error[current_rx_cell]); // callback function called at the end of a successful message reception
                    }
                }
            }
        }
    }

	sei();

}

/**
 * @brief This function computes the distance to the sender, from the two measurements performed during reception of the IR message, i.e. "LGAIN" and "HGAIN".
 * Both values are contained in the distance_measurement_t "dist" struct. The computation of the distance to the sender is performed by the user after reception.
 *
 * @param dist	Pointer to the distance measurement struct.
 * @param c     cell number where the measurement took place. This is mandatory, since every subcell has its own calibration values.
 */
uint8_t estimate_distance_mm(const distance_measurement_t *dist, cell_num_t c) {
    uint8_t i;
    uint8_t index_high=13;
    uint8_t index_low=255;
    uint8_t dist_high=255;
    uint8_t dist_low=255;
	double slope = 0;
	double b = 0;

    if (dist->high_gain < 900) {
        if (dist->high_gain > cell_irhigh[c][0]) {
            dist_high = 0;
        }
		else {
            for (i=1; i<14; i++) {
                if (dist->high_gain > cell_irhigh[c][i]) {
                    index_high = i;
                    break;
                }
            }

            slope     = (cell_irhigh[c][index_high]-cell_irhigh[c][index_high-1])/0.5;
            b         = (double)cell_irhigh[c][index_high]-(double)slope*((double)index_high*(double)0.5+(double)0.0);
            b         = (((((double)dist->high_gain-(double)b)*(double)10)));
            b         = ((int)((int)b/(int)slope));
            dist_high = b;
        }
    }

    if (dist->high_gain > 700) {
        if (dist->low_gain > cell_irlow[c][0]) {
            dist_low=0;
        }
		else {
            for(i=1; i<14; i++) {
                if(dist->low_gain > cell_irlow[c][i]) {
                    index_low = i;
                    break;
                }
            }

            if(index_low == 255) {
                dist_low=90;
            }
			else {
                slope    = (cell_irlow[c][index_low]-cell_irlow[c][index_low-1])/0.5;
                b        = (double)cell_irlow[c][index_low]-(double)slope*((double)index_low*(double)0.5+(double)0.0);
                b        = (((((double)dist->low_gain-(double)b)*(double)10)));
                b        = ((int)((int)b/(int)slope));
                dist_low = b;
            }
        }
    }

    if (dist_low != 255) {
        if (dist_high != 255) {
            return 33 + ((double)dist_high*(900.0-dist->high_gain)+(double)dist_low*(dist->high_gain-700.0))/200.0;
        }
		else {
            return 33 + dist_low;
        }
    }
	else {
        return 33 + dist_high;
    }
}
