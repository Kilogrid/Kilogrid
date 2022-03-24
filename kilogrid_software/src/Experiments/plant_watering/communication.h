#ifndef COMMUNICATION_H
#define COMMUNICATION_H


enum {
  MESSAGE_GRADIENT = 0x21,
  MESSAGE_WATER_DROP = 0x23,
  MESSAGE_TRACKING = 0x10   // corresponds to IR_message_type_t::TRACKING defined in Common/message.h
};

const double RW_FORWARD_MU		  		= 20.0;
const double RW_FORWARD_SIGMA				= 1.5;
const double RW_ROTATION_CONST				= 0.5;
const double RW_ROTATION_SPAN				= 3.0;

#endif
