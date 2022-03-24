/** \file moduleIR.h
 *
 * @brief Contains macros for timer manipulation...
*/

#ifndef KILOGRID_MODULE_IR_H
#define KILOGRID_MODULE_IR_H

#include "message.h"
#include "message_send.h"
#include "module.h"
#include "IO.h"

// Timers
#define tx_timer_setup() {\
	TCCR0A = 0;						/* Normal operation. */\
	TCCR0B = 0;						/* Set prescaler to 0 (disabled). */\
	OCR0A  = 0xFF;                  /* Set compare register to 255 (~32.768 ms). */\
	TIMSK0 |= (1<<OCIE0A);           /* Interrupt enable on match output compare register A. */\
	tx_timer_off();\
}

#define tx_timer_on() {\
	TCNT0  = 0;             		/* Reset counter. */\
	TCCR0B |= (1<<CS02)|(1<<CS00);   /* Set prescaler to 1024 (~ 128us). */\
}

#define tx_timer_off() {\
	TCCR0B = 0;				/* Set prescaler to 0 (disabled). */\
	TCNT0  = 0;             /* Reset counter. */\
}

#define tx_timer_set_compare(c) {\
	OCR0A  = c;				/* Set compare register to c. */\
	TCNT0  = 0;				/* Reset counter. */\
}

#define rx_timer_setup() {\
	TCCR1A = 0;				/* Normal operation. */\
	TCCR1B = 0;             /* Set prescaler to 0 (disabled). */\
	OCR1A  = rx_msgcycles;  /* Set compare register to rx_msgcycles (~2.96ms). */\
	TIMSK1 |= (1<<OCIE1A);  /* Interrupt enable on match output compare register A. */\
	rx_timer_off();\
}

#define rx_timer_on() {\
	TCNT1  = 0;             /* Reset counter. */\
	TCCR1B |= (1 << CS10);  /* Set prescaler to 8 (~ 1us). */\
}

#define rx_timer_off() {\
	TCCR1B = 0;             /* Set prescaler to 0 (disabled). */\
	TCNT1  = 0;             /* Reset counter. */\
}

#define rx_mux_timer_setup() {\
	TCCR2A |= (1 << WGM21);		/* CTC operation (clear timer on compare match). */\
	TCCR2B = 0;             	/* Set prescaler to 0 (disabled). */\
	OCR2A  = 79;           		/* Set compare register to 79 (~ 10.24ms). */\
	TIMSK2 |= (1 << OCIE2A); 	/* Interrupt enable on match output compare register A */\
	rx_mux_timer_off();\
}

#define rx_mux_timer_on() {\
	TCNT2  = 0;										  /* Reset counter. */\
	TCCR2B |= (1 << CS20) | (1 << CS21) | (1 << CS22); /* Set prescaler to 1024 (~ 128us). */\
}

#define rx_mux_timer_off() {\
	TCCR2B = 0;             /* Set prescaler to 0 (disabled). */\
 	TCNT2  = 0;              /* Reset counter. */\
}

/**
 * @brief Select IR receiver signal.
 * "c" denotes the cell number, and "g" the gain of the input signal (LGAIN = 0, HGAIN = 1)
 * The LGAIN positions correspond to the EVEN outputs on the MUX and the HGAIN to the ODD outputs.
 */
#define SELECT_IR_RECEIVER(c, g)    IR_RX_MUX_SELECT(((c)*2 + (g)));

/**
 * @briefSelect IR transmitter source.
 * "c" denotes the cell number.
 *
 */
#define SELECT_IR_TRANSMITTER(c)    IR_TX_MUX_SELECT((c))


#define TICKS_PER_SEC           32 ///< actual value is 1000 ms / 32.768 ms = 30.51 ticks per sec (in case OCR = 0xFF, and prescaler = 1024). We round up to the first power of 2
#define EEPROM_IRLOW            (uint8_t*)0x20
#define EEPROM_IRHIGH           (uint8_t*)0x50

#define SEC_TO_TICKS(a)         (a) * TICKS_PER_SEC

/** Number of clock cycles per bit. */
#define rx_bitcycles            269

/** Number of clock cycles for an entire message. */
#define rx_msgcycles            (11 * rx_bitcycles)

/**** TYPEDEFS AND ENUMS ****/

/*
 * @brief This structure contains the high and low gain IR receiver measurements. These measurements are used to compute the distance to the emitter.
 */
typedef struct {
    int16_t low_gain;  ///< Low gain 10-bit signal-strength measurement.
    int16_t high_gain; ///< High gain 10-bit signal-strength measurement.
} distance_measurement_t;

/** typedef for the module_IR_message_rx callback */
typedef void (*IR_message_rx_t)(IR_message_t *, cell_num_t, distance_measurement_t *, uint8_t);
/** typedef for the module_IR_message_tx_success callback */
typedef void (*IR_message_tx_success_t)(cell_num_t);

/*
 * @brief Module revision A: the even positions of output signals of the MUX correspond to the LGAIN signals
 * and the odd positions correspond to the HGAIN signals (1 receiver per cell).
 */
typedef enum{
    LGAIN = 0x00,
    HGAIN = 0x01
} IR_gain;

/**** VARIABLES ****/


/**** PROTOTYPES AND CALLBACKS ****/

/**
 * @brief Callback for message reception.
 *
 * This callback is triggered every time a message is successfully
 * decoded. The callback receives two parameters, a pointer to the
 * message decoded, and a pointer to the distance measurements from the
 * message originator.
 *
 * @code
 *  uint8_t recvd_message = 0;
 *
 *  // receive message callback
 *  void rx_message(message_t *msg, distance_measurement_t *d) {
 *      recvd_message = 1;
 *  }
 *
 *  void setup() {
 *      recvd_message = 0;
 *  }
 *
 *  // blink green when a new message is received
 *  void loop() {
 *      if ( recvd_message ) {
 *          recvd_message = 0;
 *          set_color(RGB(0,1,0));
 *          delay(100);
 *          set_color(RGB(0,0,0));
 *      }
 *  }
 *
 *  int main() {
 *      module_init();
 *      // register message callback
 *      module_message_rx = rx_message;
 *      module_start(setup, loop);
 *
 *      return 0;
 *  }
 * @endcode
 *
 * @note You must register a message callback before calling module_start.
 * @see message_t, message_crc, module_message_tx_success
 */
extern IR_message_rx_t module_IR_message_rx;

/**
 * @brief Callback for successful message transmission.
 *
 * This callback is triggered every time a message is sent
 * successfully. It receives no parameters and returns no values.
 *
 * @warning The message subsystem has no acknowledgements, therefore
 * successful message reception is not guaranteed. Instead the
 * successful message callback is called when a message is transmitted
 * and no contention is detected on the channel.
 *
 * @see message_t, message_crc, module_message_tx_success
 */
extern IR_message_tx_success_t module_IR_message_tx_success;

void init_module_IR(void);
void init_IR_message(IR_message_t *m);
void set_IR_message(IR_message_t*, cell_num_t);
void disable_IR_tx(cell_num_t);
uint8_t estimate_distance_mm(const distance_measurement_t*, cell_num_t);

#endif  // KILOGRID_MODULE_IR_H
