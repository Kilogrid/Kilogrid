/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Simple AVR demonstration.  Controls a LED that can be directly
 * connected from OC1/OC1A to GND.  The brightness_t of the LED is
 * controlled with the PWM.  After each period of the PWM, the PWM
 * value is either incremented or decremented, that's all.
 *
 * $Id: demo.c,v 1.9 2006/01/05 21:30:10 joerg_wunsch Exp $
 */

// We assume that a subcell has only one color, determined by its three components (RED, BLUE, GREEN)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "I2C_master.h"
#include "module.h"
#include "moduleLED.h"


#define PCA9635_I2C_ADDR        0x15 ///< Address of the LED driver
#define LED_UPPER_REG_LIMIT     0x0D ///< LED11 PWM signal register address is 0x0D
#define LED_LOWER_REG_LIMIT     0x02 ///< LED0 PWM signal register address 0x02
#define N_LED                   12   ///< Number of LED signals on the board

/**
 * @brief Macro that returns the LED driver register number that corresponds to an LED of cell
 * n is the cell number, and c the RGB color component (either R, G or B).
 * This macro assumes that the LEDs are connected to the LED driver in this way: R,G,B,R,G,B,...
 *
 * @param n  cell identifier
 * @param c  RGB color component
 */
#define GET_LED_REG_ADDR(n, c)  (n) * 3 + (c) + LED_LOWER_REG_LIMIT


/**
 * @brief This function provides access to all LED signals routed on the cell.
 * This provides a fine control on the brightness of every LED accessed separately.
 *
 * @note There are two ways to turn off an LED: either b == 0 (OFF) or rgb_component == 0
 *
 * @param n     Number of the subcell.
 * @param c	    Color of the LED. A color is coded as a 3-bit binary number that represents the RGB values (ON or OFF).
 *              A consequence of this is that there are only 8 possible combinations of colors with this function.
 * @param b	    Brightness, an 8 bit unsigned integer.
 */
static inline void change_LED_state(cell_num_t n, color_t c, brightness_t b) {
	uint8_t rgb_component; // binary that will successively store R, G and B of the c color
	uint8_t LED_reg_addr; // contains the address of the PWM register to access
	uint8_t i; // counter

	// write each color component of c in the LED driver registers
	for(i = 0; i < 3; i++) {

		rgb_component = (c & (1 << i)) >> i; // get the i-th RGB component contained in c (either 0 or 1)

		LED_reg_addr = GET_LED_REG_ADDR(n, i); // get the address of the current rgb component of subcell s

		// Ensure that we only change the first five bits of the control register of the PCA9635
		// and that the changed registers correspond to actual LEDs on the board.
		if(LED_reg_addr <= LED_UPPER_REG_LIMIT && LED_reg_addr >= LED_LOWER_REG_LIMIT){
			I2C_start((PCA9635_I2C_ADDR << 1) + I2C_WRITE);

			//Control register - Autoincrement OFF
			I2C_write(0x00 | LED_reg_addr); // write address of register on the I2C bus
			I2C_write(b * rgb_component); // write the value of the PWM (brightness) - will turn the LED off if rgb_component == 0

			I2C_stop();
		}
		// else: no change to the LED driver

	} // end for


} // end function


/**
 * @brief This function provides access to all LED signals routed on the cell.
 * This provides a fine control on the brightness of every LED accessed separately.
 *
 * @note There are two ways to turn off an LED: either b == 0 (OFF) or rgb_component == 0
 *
 * @param n     Number of the subcell.
 * @param c	    Color of the LED. A color is coded as a 3-bit binary number that represents the RGB values (ON or OFF).
 *              A consequence of this is that there are only 8 possible combinations of colors with this function.
 * @param b	    Brightness, an 8 bit unsigned integer.
 */
static inline void change_LED_state_hex(cell_num_t n, uint8_t c[]) {
	uint8_t rgb_component; // binary that will successively store R, G and B of the c color
	uint8_t LED_reg_addr; // contains the address of the PWM register to access
	uint8_t i; // counter

	// write each color component of c in the LED driver registers
	for(i = 0; i < 3; i++) {

		//rgb_component = (c & (1 << i)) >> i; // get the i-th RGB component contained in c (either 0 or 1)
		rgb_component = c[i];

		LED_reg_addr = GET_LED_REG_ADDR(n, i); // get the address of the current rgb component of subcell s

		// Ensure that we only change the first five bits of the control register of the PCA9635
		// and that the changed registers correspond to actual LEDs on the board.
		if(LED_reg_addr <= LED_UPPER_REG_LIMIT && LED_reg_addr >= LED_LOWER_REG_LIMIT){
			I2C_start((PCA9635_I2C_ADDR << 1) + I2C_WRITE);

			//Control register - Autoincrement OFF
			I2C_write(0x00 | LED_reg_addr); // write address of register on the I2C bus
			I2C_write(rgb_component); // write the value of the PWM (brightness) - will turn the LED off if rgb_component == 0

			I2C_stop();
		}
		// else: no change to the LED driver

	} // end for


} // end function


/**
 * @brief Initialization of the I2C bus and of the LED driver registers.
 */
void init_module_LED(void) {

	uint8_t i = 0; // counter

	I2C_init();

	I2C_start((PCA9635_I2C_ADDR << 1) + I2C_WRITE);

	//Autoincrement ALL registers, start at reg 0 - we're initiating
	I2C_write(0x80);

	//Reg 0x00 = MODE1 - set to 0x80 - autoincrement enabled (this is readonly?), do not respond to subaddresses or allcall
	I2C_write(0x80);

	//Reg 0x01 = MODE2 - set to 0x02 - noninverted logic state, open-drain
	I2C_write(0x02);

	//Reg 0x02-0x11 - PWM0-15 - LED brightnesses, start at 0 brightness

	for(i = 0; i < 16; i++){
		I2C_write(0x00);
	}

	//Reg 0x12 - Group PWM - should not matter
	I2C_write(0xff);

	//Reg 0x13 - Group Freq - should not matter
	I2C_write(0x00);

	//Reg 0x14-0x17 - LED Output State Control - all 0xAA (output state for each LED = 0x2)
	I2C_write(0xaa);
	I2C_write(0xaa);
	I2C_write(0xaa);
	I2C_write(0xaa);

	//Reg 0x18-0x1b - Subaddressing stuff, doesn't matter, just stop the transfer
	I2C_stop();

}

/**
 * @brief Set a specified cell's LEDs with specified brightness.
 *
 * @param	n	cell
 * @param	c	color
 * @param	b	brightness
 */
void set_LED_with_brightness(cell_num_t n, color_t c, brightness_t b) {
	change_LED_state(n, c, b);
}

/**
 * @brief Set a specified cell's LEDs to a color at ADJUSTED brightness.
 *
 * @param	n		cell
 * @param	c		color
 */
void set_LED(cell_num_t n, color_t c) {
    set_LED_with_brightness(n, c, ADJUSTED);
}


/**
 * @brief Set all LEDs color to a color and brightness.
 *
 * @param	c		color
 * @param	b		brightness
 */
void set_all_LEDs_with_brightness(color_t c, brightness_t b) {
    for(uint8_t n = CELL_LOWER_INDEX; n <= CELL_UPPER_INDEX; ++n) {
        set_LED_with_brightness(n, c, b);
    }
}


/**
 * @brief Set all LEDs color to a color and brightness.
 *
 * @param	c		color
 * @param	b		brightness
 */
void set_all_LEDs_hex(uint8_t c[]) {
    for(uint8_t n = CELL_LOWER_INDEX; n <= CELL_UPPER_INDEX; ++n) {
        change_LED_state_hex(n, c);
    }
}


/**
 * @brief Set all LEDs color, at ADJUSTED brightness.
 *
 * @param	c	color
 */
void set_all_LEDs(color_t c){
    set_all_LEDs_with_brightness(c, ADJUSTED);
}


