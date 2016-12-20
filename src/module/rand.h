/** \file rand.h */

// TODO Credits to Harvard kilobot v2!!!!!

#ifndef __RAND_H__
#define __RAND_H__

#include "stdint.h"

/**
 * @brief Hardware random number generator.
 *
 * This function uses the analog-to-digital converter to generate an
 * 8-bit random number. Specifically, the robot measures its own battery
 * voltage and extracts randomness from the least significant bit by
 * using Von Neumann's fair-coin algorithm. By its nature, this function
 * is slow, use rand_soft() if you want a faster alternative, and you
 * can seed the software random generator using the output of
 * rand_hard().
 *
 * @see rand_soft, rand_seed
 * @return 8-bit random number.
 */
uint8_t rand_hard(void);

/**
 * @brief Software random number generator.
 *
 * This function implements a linear-shift-register to implement an
 * 8-bit pseudo-random number generator. The seed of the random number
 * generator can be controlled through rand_seed().
 *
 * @return 8-bit random number.
 */
uint8_t rand_soft(void);
/**
 * @brief Seed software random number generator.
 *
 * This function changes the seed used by the software random
 * number generator implemented by rand_soft().
 *
 * @param 8-bit random seed.
 */
void rand_set_seed(uint8_t);

#endif //__RAND_H__