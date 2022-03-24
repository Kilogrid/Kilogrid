#ifndef __KILO_BEHAVIORS_COMMON_H_
#define __KILO_BEHAVIORS_COMMON_H_

/**
 * @brief Types of IR messages for kilob
 *
 * This list should be compatible with IR_message_type_t defined in Common/message.h.
 */
typedef enum {
    TRACKING_MSG_TYPE = 0x10,
    GRADIENT_MSG_TYPE = 0x21
} kilob_message_type;

#endif
