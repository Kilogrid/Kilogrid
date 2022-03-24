#ifndef __KILOB_RANDOM_WALK_H__
#define __KILOB_RANDOM_WALK_H__

// allows for debugging the random walk and wall avoidance behaviors
//#define ENABLE_LIGHTS_RW_WA

void kilob_random_walk();
void kilob_random_walk_init(double _forward_mu_sec, double _forward_sigma_sec, double _rotation_const_sec, double _rotation_span_sec);
void kilob_random_walk_reset();

#endif
