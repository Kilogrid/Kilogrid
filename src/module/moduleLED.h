/** \file moduleLED.h */
#ifndef KILOGRID_MODULE_LED_H
#define KILOGRID_MODULE_LED_H

#include <stdint.h>
#include "module.h"


/**
 * @brief Color components of every color based on their RGB value.
 * The LSB indicates the R component, and the MSB the B component.
 */
typedef enum {
    RED     = 0b001,
    GREEN   = 0b010,
    BLUE    = 0b100,
    YELLOW  = 0b011,
    CYAN    = 0b110,
    MAGENTA = 0b101,
    WHITE   = 0b111,
    LED_OFF = 0b000
} color_t;

/** @brief Brightness constants. */
typedef enum {
    ADJUSTED = 0x20,            ///< Adjusted brightness for camera
    LOW      = 0x05,            ///< Lower-bound brightness
    HIGH     = 0xF0,            ///< Higher-bound brightness
    MEDIUM   = (LOW + HIGH)/2   ///< Medium brightness
} brightness_t;


void init_module_LED(void);

void set_LED(cell_num_t, color_t);
void set_all_LEDs(color_t);
void set_all_LEDs_hex(uint8_t c[]);

void set_LED_with_brightness(cell_num_t, color_t, brightness_t);
void set_all_LEDs_with_brightness(color_t, brightness_t);

#endif // KILOGRID_MODULE_LED_H
