#ifndef COMMUNICATION_H
#define COMMUNICATION_H

enum {
  MESSAGE_ADVERTISE_KILOBOT_OPINION = 0x00,
  MESSAGE_FOR_TRACKING = 0x10,   // corresponds to IR_message_type_t::TRACKING defined in Common/message.h
  MESSAGE_ADVERTISE_CELL_STATE
};

enum {
    INDEX_CELL_STATE = 0,
    INDEX_WALL_FLAG     = 1
};

#endif  /* COMMUNICATION_H */
