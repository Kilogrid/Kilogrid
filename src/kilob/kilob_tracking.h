/**
 * @file     Kilogrid/src/kilob/kilob_tracking.h
 * @brief    Library of function for the Kilobot robot. 
 * The kilob tracking aims to allow users to send tracking position of the 
 * Kilobot via infrared.
 * @details  This library provides an interface to send tracking position via 
 * Kilobot's infrared.
 *
 * This library provides two functions (::kilob_tracking_init and 
 * ::kilob_tracking) to send an infrared messages towards the cells of the
 * Kilogrid arena (i.e. in the infrared range) which contains a tracking 
 * position message. The message is composed by user specified data, for
 * example the id of the Kilobot as in kilobot_controller.c.
 * 
 * Once received by a cell the tracking message can then be completed with 
 * coordinates of the cell and transmitted to the dispatcher. The dispatcher 
 * will then transmit the information to the KiloGUI available to the user.
 * Note that the cell has to be programmed to receive and transmit tracking
 * information, please refer to module_controller.c for an example.
 * 
 * @author   IRIDIA lab
 * @date     March 2017
 */
#ifndef __KILOB_TRACKING_H__
#define __KILOB_TRACKING_H__

#include <stddef.h>
#include <stdint.h>
#include "kilogrid.h"
#include "kilob_common.h"
#include "kilob_messaging.h"
#include "kilolib.h"
#include "utils.h"

/**
 * @brief Initialize tracking library.
 * @details Initialize a timer and the messaging library.
 */
void kilob_tracking_init();

/**
 * @brief This function implements the tracking functionality. 
 * @details It sends a message to the cell every TR_TIMEOUT milliseconds. If 
 * the message buffer is full, it will cancel the current message.
 */
void kilob_tracking(tracking_user_data_t* data);

#endif
