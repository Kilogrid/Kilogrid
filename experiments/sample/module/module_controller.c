/**
 * @file     Kilogrid/experiments/sample/module/module_controller.c
 * @brief    This file implements a sample code of controller for a module of
 * the Kilogrid arena.
 *
 * @details    
 * @details  In this controller, the module is programmed to send a tracking 
 * message received via the infrared interface to the dispatcher via the CAN  
 * interface. Note that the tracking message is automatically transmitted to the
 * KiloGUI via the USB interface.
 * 
 * This code can be used as a template for developing your own more complex
 * controller. 
 * 
 * It shows how to init, setup and run a program on the module. It also shows 
 * how to use the infrared reception callback interface, transmission is not 
 * use here but it is also possible. 
 * \n It also shows how to initialize and send a CAN message towards the 
 * dispatcher. Note that CAN message can also be received but it is not shown 
 * here.
 *  
 * The user has access to different function via the provided libraries
 * (e.g. LED, Infrared, CAN) to control elements of the module. Please refer to
 * the documentation of the module folder for more details.
 * 
 * @author   IRIDIA lab
 * @date     March, 2017
 * @copyright licensed under creative commons attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0) 
 *           more info at http://creativecommons.org/licenses/by-nc-sa/3.0/
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>  // required if you need delay macro

#include <CAN.h>
#include <debug.h>
#include <module.h>
#include <moduleIR.h>
#include <moduleLED.h>


/**
 * @brief Basic tacking data handler.
 *
 * @param m  the tracking message
 * @param c  the cell which receives the message
 */
void send_tracking(IR_message_t *m, cell_num_t c)
{
    // get CAN message buffer
    CAN_message_t *msg = next_CAN_message();

    // send tracking message unless the buffer is full
    if(msg != NULL) {
        serialize_tracking_message(msg, c, (tracking_user_data_t*)m->data);
    }
}


/**
 * @brief Callback function called when IR message is received.
 *
 * @param m  the message
 * @param c  the cell which receives the message
 * @param d  the estimated distance between the cell and the message sender
 * @param CRC_error  the error flag
 */
void IR_rx(IR_message_t *m, cell_num_t c, distance_measurement_t *d,
           uint8_t CRC_error)
{
    /* Basic implementation */
    if(CRC_error) return;       // ignore message with error

    if(m->type == TRACKING) {   // handle tracking message
        send_tracking(m, c);
    }

    /* [TODO] Put your message handling code */
}


/**
 * @brief Setup module controller.
 * @details This function will be called one time at the start of the module.
 *  It is used to initialize the module, for example initializing the state of
 *  a module w.r.t. its ID.
 */
void setup()
{
    /* [TODO] Put your setup code */
}


/**
 * @brief Main loop of module controller.
 * @details This callback function is a main loop that runs endlessly on the 
 * module. Experiment code should be put inside this function. For example, a 
 * state machine that sends specific messages to kilobot w.r.t. states.
 */
void loop()
{
    /* [TODO] Put your experiment code */
}


int main() {
    // [required] initialize module
    // This function should be called first
    module_init();

    // [option] register message receiving callback
    // This code should be put before calling module_start
    module_IR_message_rx = IR_rx;

    // [required] start module controller
    module_start(setup, loop);
    return 0;
}
