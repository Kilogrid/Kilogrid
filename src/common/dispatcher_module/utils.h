/**
 * @file     Kilogrid/src/common/dispatcher_module/utils.h
 * @brief    This file provides definitions for bit access.
 *   
 * @author   Anthony Antoun, IRIDIA lab
 * @date     May, 2017
 */

#ifndef UTILS_H_
#define UTILS_H_

#define BIT_IS_SET(byte,bit)		(byte & (1 << bit)) != 0
#define BIT_IS_CLEAR(byte,bit)		(byte & (1 << bit)) == 0 

#define GET_BIT(byte,bit)			(byte & (1 << bit)) >> bit
#define SET_BIT(byte,bit)			byte |= (1 << bit)

#define false						0 
#define true						1

#endif /* UTILS_H_ */