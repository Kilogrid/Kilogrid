#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>     // delay macros
#include <avr/eeprom.h>     // read eeprom values
#include <avr/boot.h>       // to write boot pages
#include <avr/interrupt.h>
#include <string.h>        // for memcpy

#include <debug.h>
#include <bitfield.h>
#include <CAN.h>
#include <mcp2515.h>

#include "module.h"
#include "moduleLED.h"

#define EEPROM_VALID_APPLICATION_BIT_ADDR           (uint8_t*)0xB4
#define CAN_LAST_BOOTPAGE_MESSAGE_REMAINING_BYTES   ((BOOTPAGE_SIZE+1) % CAN_MESSAGE_PAYLOAD)


// TEST ONLY
//#define TEST_CAN_BEACON
//#define ERASE_FLASH
//#define BOOTLOAD_ALWAYS // always start by the bootloading program, even if a valid application is present in memory

static CAN_message_t CAN_msg_rx;
static volatile CAN_message_type_t CAN_msg_type;
static uint8_t current_CAN_msg_number;
static volatile uint16_t CAN_msg_counter;		  // counter for the total number of CAN messages received

static uint8_t page_total;
static uint8_t page_count;
static uint8_t page_address;
static uint8_t page_offset;
static volatile uint16_t page_byte_count;

static uint8_t fBootload_finished;

static uint16_t page_buffer[SPM_PAGESIZE/2 + 2];

static volatile uint8_t debug_1;

BF_create(page_table, 224);

void goto_program() {
	MCUCR = (1<<IVCE); // interrupts moved to the main application
	MCUCR = 0;
	eeprom_write_byte(EEPROM_VALID_APPLICATION_BIT_ADDR, 0x01); // indicate that a valid application is present at the beginning of the FLASH
	asm volatile ("jmp 0x0000");
}

/*
 *	@brief Callback function triggered by the CAN library after the reception of a CAN message.
 *  The code of this function is prioritary compared to the rest of the program, and its execution cannot be slowed down or interrupted, including by the reception of a new message.
 *  As a consequence, it is not recommended to call any of the CellLEDs function during this function.
 *  Note: goto_program() is ineffective in this callback and can also lead to unpredictable behavior.
 *
 */
void CAN_rx(CAN_message_t *m){

	CAN_msg_rx = *m; // copy message into CAN_msg_rx buffer struct
	CAN_msg_type = CAN_msg_rx.data[0];

	CAN_msg_counter++;

	current_CAN_msg_number = CAN_msg_rx.data[1];

	switch(CAN_msg_type){

		case CAN_MODULE_BOOT:
			asm volatile ("jmp 0x7000"); // jump to bootloader code
			break;

		case CAN_MODULE_BOOTPAGE:
			// current_CAN_msg_number is the bootpage data offset, in bytes
			page_offset = (current_CAN_msg_number * 6) >> 1; // offset of the current data according to the bootpage, in words

			// Fill in page buffer
			// The buffer contains words (uint16_t)
			if(current_CAN_msg_number < (CAN_MESSAGES_PER_BOOTPAGE-1)){ // if the message is not the last CAN message of the bootpage
				memcpy(page_buffer + page_offset, &CAN_msg_rx.data[2], CAN_MESSAGE_PAYLOAD);
				page_byte_count += CAN_MESSAGE_PAYLOAD;
			}
			else if(current_CAN_msg_number == (CAN_MESSAGES_PER_BOOTPAGE-1)){ // if the message is the last CAN message of current bootpage: only copy the two remaining words (128 - 124 = 4 bytes = 2 words)
				memcpy(page_buffer + page_offset, &CAN_msg_rx.data[2], CAN_LAST_BOOTPAGE_MESSAGE_REMAINING_BYTES);
				page_byte_count += CAN_LAST_BOOTPAGE_MESSAGE_REMAINING_BYTES;
			}
			//else: ignore message (considered as erroneous)

			if( page_byte_count >= SPM_PAGESIZE && (!BF_get(page_table, page_address)) ){ // bootpage received and bootpage not already in memory

				page_byte_count = 0; // reset byte counter

				// Write program page to flash
				// Taken from http://www.nongnu.org/avr-libc/user-manual/group__avr__boot.html

				eeprom_busy_wait();

				boot_page_erase(page_address*SPM_PAGESIZE);
				boot_spm_busy_wait();

				for (uint8_t i = 0, j = 0; i < SPM_PAGESIZE; i += 2, j++){
					boot_page_fill(page_address*SPM_PAGESIZE+i, page_buffer[j]);
				}

				boot_page_write(page_address*SPM_PAGESIZE);
				boot_spm_busy_wait();

				boot_rww_enable();

				BF_set(page_table, page_address); // set flag to 1 in bitfield to indicate the presence of the bootpage in memory

				page_count++;

				if(page_count == page_total){
					fBootload_finished = 1;
				}

			}

			break;

		case CAN_MODULE_BOOTPAGE_SIZE:

			page_total = CAN_msg_rx.data[2];

			if (page_count == page_total){
				fBootload_finished = 1;
			}

			break;

		case CAN_MODULE_BOOTPAGE_NUMBER:
			page_address = CAN_msg_rx.data[2]; // store the address of the bootpage in memory
			break;

		case CAN_MODULE_IDLE:
			// return to the application code
			if(eeprom_read_byte(EEPROM_VALID_APPLICATION_BIT_ADDR) == 1){
				fBootload_finished = 1;
			}
			break;

		// we also want to go back to user application mode when the user presses on the RUN or SETUP buttons
		case CAN_MODULE_SETUP:
			// return to the application code
			if(eeprom_read_byte(EEPROM_VALID_APPLICATION_BIT_ADDR) == 1){
				goto_program();
			}
			break;

		case CAN_MODULE_RUN:
			// return to the application code
			if(eeprom_read_byte(EEPROM_VALID_APPLICATION_BIT_ADDR) == 1){
				goto_program();
			}
			break;

		default:
			break;

	}

}

// user-defined callback for CAN message transmission
void CAN_tx_success(uint8_t success){
    /* Dummy */
}

int main() {

	cli(); // clear interrupts

	// move interrupt vectors to bootloader interupts
	//MCUCR = (1<<IVCE);
	//MCUCR = (1<<IVSEL);
	// same instructions, but quicker
	MCUCR = 0x01; // IVCE = 1
	MCUCR = 0x02; // IVSEL = 1, IVCE = 0

	BF_init(page_table);
	page_total = 220;
	page_count = 0;
	page_address = 0;
	page_byte_count = 0;
	debug_1 = 0;
	page_offset = 0;
	fBootload_finished = 0;

	init_CAN_message(&CAN_msg_rx);

	sei(); // enable interrupts

	module_CAN_message_rx = CAN_rx;

#ifdef ERASE_FLASH
	// erase FLASH application code (only when debugging)
	for(uint8_t j = 0; j < 7000; j += SPM_PAGESIZE){
		boot_page_erase_safe(j);
	}
#endif

	// initialize all peripherals of the cell
    module_init();


    // main loop
    while(1){

		if(fBootload_finished == 1){
			goto_program();
		}

		set_all_LEDs(BLUE);
		_delay_ms(200);
		set_all_LEDs(LED_OFF);
		_delay_ms(200);
	} // while 1

    return 0;
}
