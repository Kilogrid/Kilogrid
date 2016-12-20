// TODO Credits to Harvard kilobot v2!!!!!

#include <avr/interrupt.h>

#include "rand.h"
#include "stdint.h"
#include "ADC.h"

/**** GLOBAL VARIABLES ****/
static uint8_t seed=0xaa, accumulator = 0;

/**** FUNCTIONS ****/
uint8_t rand_hard(void) {
    uint8_t num = 0;
    uint8_t a, b, i, tries;
    for (i = 0; i < 8; i++) {
        tries = 0;
        do {
            cli(); // enter critical section
            ADC_SELECT_CHANNEL(6);
            ADC_START_CONVERSION();
            ADC_FINISH_CONVERSION();
            a = ADCW&0x1;
            ADC_START_CONVERSION();
            ADC_FINISH_CONVERSION();
            b = ADCW&0x1;
            //ADC_TRIGGER_HIGH_GAIN();
            sei(); // exit critical section
            tries++;
        } while (a == b && tries < 128);
        if (a)
            num |= (1<<i);
    }
    return num;
}

uint8_t rand_soft(void) {
    seed ^= seed<<3;
    seed ^= seed>>5;
    seed ^= accumulator++>>2;
    return seed;
}

void rand_set_seed(uint8_t s) {
    seed = s;
}