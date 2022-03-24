/** \file utils.h */

/*
 * \file utils.h
 *
 *  \author Anthony Antoun
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