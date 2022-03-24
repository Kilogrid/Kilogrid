#include <math.h>		
#include <stdlib.h>		// for srandom() and random())
#include <kilo_rand_lib.h>
#include <platform.h> // for cross-platform compilation

// for rand_hard()
#ifdef MODULE 
	#include "rand.h"
#elif defined(KILOBOT)
	#include "kilolib.h"	
#endif 

static uint32_t u32_seed; // longer seed, which is always reflecting (equal to) the seed in the math library 

/** @brief Gabri's initial g_ran_set_seed() function, adapted to kilolib_v2. It uses the rand_hard() function to set the uint32 seed. If a non-zero seed is provided as an argument of the function, u32_seed will be set to that seed. 
 * @arg seed
 *
 */
void g_ran_set_seed_uint32(uint32_t s) {
  
  if (s != 0) {
    u32_seed = s;
  } 
  else { // the input seed is zero: generate a new seed 
	
	// use rand_hard to set the 4 bytes of the u32 seed
	for(uint8_t i = 0; i < 4; i++){
		u32_seed = u32_seed << (i*8); // shift the seed to make the 8 LSBs empty
		u32_seed += rand_hard(); // fill in the LSBs with the result of rand_hard())
	}
  }
  srandom(u32_seed); // set the seed internally
}

double g_ran_uniform() {
  return ((double) random() / (double) RANDOM_MAX);
}

uint32_t g_ran_uniform_uint32() {
  return (uint32_t) (((double) random() / (double) RANDOM_MAX)*UINT32_MAX);
}

double g_ran_exponential(double lambda) {
  return (-1.0 / lambda * log(1.0 - g_ran_uniform()));
}

double g_ran_gaussian(double mu, double sigma) {
  static int n = 0;
  static double z0 = 0.0, z1 = 0.0;

  if (n == 0) {
    double u0,u1,t,r;

	u0 = g_ran_uniform();
	u1 = g_ran_uniform();
    r  = sqrt((double) -2.0 * log(u0));
    t  = (double) 2.0 * M_PI * u1;
    z0 = (double) r * cos(t);
    z1 = (double) r * sin(t);
	n  = 1;
	return mu + sigma * z0;
  } 
  else {
    n = 0;
	return mu + sigma * z1;
  }
}  
