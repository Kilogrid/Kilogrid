#ifndef __KILOGRID_H__
#define __KILOGRID_H__

#include <stdint.h>

typedef enum{
    KILOGRID_STOP_IR = 0xC0,
    KILOGRID_IDLE,
    KILOGRID_SETUP,
    KILOGRID_RUN_EXP,
    KILOGRID_PAUSE_EXP,
    KILOGRID_BOOTLOAD
} kilogrid_command_type_t;

typedef struct{
    const char *name;
    const kilogrid_command_type_t type;
} kilogrid_command_t;

// packed attribute ensures that the enum is implemented with the shortest integer type available (important for use with different compilers (KiloGUI))
// (We want to force the gcc compiler used in Qt to use uint8_t or similar)
// See URL https://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/attributes-types.html
// for reference.
typedef enum __attribute__ ((__packed__)) {
	ADDR_BROADCAST = 0x00, ///< broadcast to all modules
	ADDR_ROW, ///< broadcast to all modules in a row
	ADDR_COLUMN, ///< broadcast to all modules in a column
	ADDR_INDIVIDUAL, ///< send to an individual module
	ADDR_DISPATCHER = 0xFF ///< send to dispatcher
} kilogrid_addressing_type_t;

typedef struct __attribute__((__packed__)){

	uint8_t x; ///< columns
	uint8_t y; ///< rows

	kilogrid_addressing_type_t type; ///< type of address in Kilogrid

} kilogrid_address_t;

typedef enum{
	TO_MODULE = 0,
	TO_DISPATCHER = 1
} destination_type_t;

/**
 *  @brief Number of bytes available to the user inside a tracking message.
 */
#define TRACKING_MSG_USER_DATA			7

/**
 *  @brief Encapsulates a byte array to ease manipulation and assignment in all Kilogrid project files. The user has a few bytes available when sending tracking messages, for example to communicate its state, etc without sending additional messages.
 */
typedef struct __attribute__((__packed__)){
	uint8_t byte[TRACKING_MSG_USER_DATA];
} tracking_user_data_t;

static const kilogrid_command_t KILOGRID_COMMANDS[] = {
    {"Setup", KILOGRID_SETUP},
    {"Run", KILOGRID_RUN_EXP},
    {"Pause", KILOGRID_PAUSE_EXP}
};

static const int NUM_KILOGRID_COMMANDS = sizeof(KILOGRID_COMMANDS)/sizeof(kilogrid_command_t);

#endif // _KILOGRID_H_
