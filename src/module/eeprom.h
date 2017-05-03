/**
 * @file     Kilogrid/src/module/eeprom.h
 * @brief    This file defines eeprom struct for data storage.
 * 
 * @author   IRIDIA lab
 * @date     January, 2017
 */

#ifndef KILOGRID_MODULE_EEPROM_H
#define KILOGRID_MODULE_EEPROM_H

typedef enum {
	MODULE_EEPROM_X_COORD_ADDR = 0xB2,
	MODULE_EEPROM_Y_COORD_ADDR,
	MODULE_EEPROM_CALIB_RESTRICTED,
	MODULE_EEPROM_CALIB_X_MIN,
	MODULE_EEPROM_CALIB_X_MAX,
	MODULE_EEPROM_CALIB_Y_MIN,
	MODULE_EEPROM_CALIB_Y_MAX,
} module_eeprom_address_t;

#endif  /* KILOGRID_MODULE_EEPROM_H */
