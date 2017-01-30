/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Simple AVR demonstration.  Controls a LED that can be directly
 * connected from OC1/OC1A to GND.  The brightness of the LED is
 * controlled with the PWM.  After each period of the PWM, the PWM
 * value is either incremented or decremented, that's all.
 *
 * $Id: demo.c,v 1.9 2006/01/05 21:30:10 joerg_wunsch Exp $
 */

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "I2C_master.h"

const uint8_t PCA9635_i2c_addr = 0x15; // TODO: to be changed to struct later

void init_PCA9635(void)			/* Note [6] */
{
  I2C_start((PCA9635_i2c_addr << 1) + I2C_WRITE);
  
  //Autoincrement ALL registers, start at reg 0 - we're initting
  I2C_write(0x80);
  
  //Reg 0x00 = MODE1 - set to 0x80 - autoincrement enabled (this is readonly?), do not respond to subaddresses or allcall
  I2C_write(0x80);
  
  //Reg 0x01 = MODE2 - set to 0x02 - noninverted logic state, open-drain
  I2C_write(0x02);
  
  //Reg 0x02-0x11 - PWM0-15 - LED brightnesses, start at low brightness (0x05)
  
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  I2C_write(0x05);
  
  //Reg 0x12 - Group PWM - should not matter
  I2C_write(0xff);

  //Reg 0x13 - Group Freq - should not matter
  I2C_write(0x00);

  //Reg 0x14-0x17 - LED Output State Control - all 0xAA (output state for each LED = 0x2)
  I2C_write(0xaa);
  I2C_write(0xaa);
  I2C_write(0xaa);
  I2C_write(0xaa);

  //Reg 0x18-0x1b - Subaddressing stuff, doesn't matter, just stop the xfer
  I2C_stop();
  
}
