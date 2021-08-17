// TODO: add credits to kilolib v2

#ifndef __KILO_RAND_LIB_H__
#define __KILO_RAND_LIB_H__

#include <stdint.h>

/**
 * @brief Creates an integer value with gaussian distribution.
 *
 * @param mu_sec	the mean
 * @param sigma_sec	the variance
 */
#define INT_GAUSSIAN_TICKS(mu_sec, sigma_sec) (uint32_t)(floor(abs(g_ran_gaussian(mu_sec, sigma_sec))) * TICKS_PER_SEC)

// Random variates functions (srand()) MUST be called before using these..)
void g_ran_set_seed_uint32(uint32_t);       // Set the random seed, if seed=0 the seed is set randomly 
double g_ran_uniform();                  // Return a random value in the range [0,1[
uint32_t g_ran_uniform_uint32();         // Return a random value in the range [0,UINT32_MAX]
double g_ran_exponential(double);        // Return an exponential random value with rate \lambda 
double g_ran_gaussian(double, double);  // Return a Gaussian random value with mean and variance

#endif // __KILO_RAND_LIB_H__