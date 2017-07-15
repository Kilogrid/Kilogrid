/**
 * @file     Kilogrid/src/module/moduleLED.h
 * @brief    This file implements high-level handling of LED setup.
 * @details  The modules hardware is embedded with two LEDs per cells. LEDs of
 * the same cell are driven in parallel therefore show the same color.
 * 
 * This file provides a set of three functions to initialize the LED hardware 
 * (::init_module_LED) and set LED colors and brigthness
 * (::set_LED_with_brightness, ::set_all_LEDs_with_brightness).
 * 
 * The user can target a specific cell or all cells when setting up the color
 * and brightness.
 * 
 * Each LED has a resolution of 24bits (8 bits for each RGB components 
 * ::color_t) which allows user to control accurately the color desired. In 
 * addition, four levels of brightness are provided (::brightness_t).
 * 
 * @author   IRIDIA lab
 * @date     January, 2017
 */
 // Note: brigthness MEDIUM should be default in the function hence declaring
 // two function only to set up led color and brightness
#ifndef KILOGRID_MODULE_LED_H
#define KILOGRID_MODULE_LED_H

#include <stdint.h>
#include "module.h"


/**
 * @brief Color components of every color based on their RGB value.
 * The LSB indicates the R component, and the MSB the B component.
 */
typedef enum {
    RED     = 0b001,        ///< 0b001
    GREEN   = 0b010,        ///< 0b010  
    BLUE    = 0b100,        ///< 0b100   
    YELLOW  = 0b011,        ///< 0b011 
    CYAN    = 0b110,        ///< 0b110   
    MAGENTA = 0b101,        ///< 0b101
    WHITE   = 0b111,        ///< 0b111  
    LED_OFF = 0b000         ///< 0b000
} color_t;

/** @brief Brightness constants. */
typedef enum {
    ADJUSTED = 0x20,            ///< Adjusted brightness for camera
    LOW      = 0x05,            ///< Lower-bound brightness
    HIGH     = 0xF0,            ///< Higher-bound brightness
    MEDIUM   = (LOW + HIGH)/2   ///< Medium brightness
} brightness_t;

/**
 * @brief Initialization of the I2C bus and of the LED driver registers.
 */
void init_module_LED(void);

/**
 * @brief Set a specified cell's LEDs to a color at ADJUSTED brightness.
 *
 * @param   n       cell
 * @param   c       color
 */
void set_LED(cell_num_t n, color_t c);

/**
 * @brief Set all LEDs color, at ADJUSTED brightness.
 *
 * @param   c   color
 */
void set_all_LEDs(color_t c);

/**
 * @brief Set a specified cell's LEDs with specified brightness.
 *
 * @param   n   cell
 * @param   c   color
 * @param   b   brightness
 */
void set_LED_with_brightness(cell_num_t n, color_t c, brightness_t b);

/**
 * @brief Set all LEDs color to a color and brightness.
 *
 * @param   c       color
 * @param   b       brightness
 */
void set_all_LEDs_with_brightness(color_t c, brightness_t b);

#endif // KILOGRID_MODULE_LED_H
