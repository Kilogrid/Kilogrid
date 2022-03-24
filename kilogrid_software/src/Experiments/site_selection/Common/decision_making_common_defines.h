#ifndef __DECISION_MAKING_COMMON_DEFINES_H_
#define __DECISION_MAKING_COMMON_DEFINES_H_

#include <stdint.h>
#include "utils.h"

const uint8_t MAJORITY_RULE_GROUP_SIZE		= 5;

const double SITE_A_EXPLORE_RAND_PARAM		= 1/(7.8*60.0); // exponential parameter
const double SITE_B_EXPLORE_RAND_PARAM		= 1/60.0; // exponential parameter
const double MODULATION_G_PARAM				= 10*60.0;
const double MODULATION_EXP_SCALING			= 5.0;

const double QUALITY_MU_SITE_A				= 1.0;
const double QUALITY_SIGMA_SITE_A			= 0.1;
const double QUALITY_MU_SITE_B				= 0.9;
const double QUALITY_SIGMA_SITE_B			= 0.1;

const uint8_t GRADIENT_IDX_WALL				= 3;
const uint8_t GRADIENT_IDX_TO_SITE_A		= 0;
const uint8_t GRADIENT_IDX_TO_SITE_B		= 1;
const uint8_t GRADIENT_IDX_TO_NEST			= 2;

const uint8_t GRADIENT_ID_WALL 				= 0x01;
const uint8_t GRADIENT_ID_TO_SITE_A 		= 0x02;
const uint8_t GRADIENT_ID_TO_SITE_B 		= 0x03;
const uint8_t GRADIENT_ID_TO_NEST 			= 0x04;

const uint8_t AREA_TYPE_NEST				= 0x02;
const uint8_t AREA_TYPE_SITE_A				= 0x03;
const uint8_t AREA_TYPE_SITE_B				= 0x01;

const double GRADIENT_CHECK_PERIOD			= 5.0;
const double GRADIENT_TURN_SMALL_MU			= 2.0;
const double GRADIENT_TURN_SMALL_SIGMA		= 0.5;
const double GRADIENT_TURN_LARGE_MU			= 4.0;
const double GRADIENT_TURN_LARGE_SIGMA		= 1.0;
const double GRADIENT_STUCK_TIMEOUT			= 30.0;

const double RW_SITE_FORWARD_MU		  		= 15.0;
const double RW_SITE_FORWARD_SIGMA			= 1.5;
const double RW_NEST_FORWARD_MU		  		= 25.0;
const double RW_NEST_FORWARD_SIGMA			= 1.5;
const double RW_ROTATION_CONST				= 0.5;
const double RW_ROTATION_SPAN				= 3.0;

#endif