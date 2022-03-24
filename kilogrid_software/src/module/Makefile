##########------------------------------------------------------##########
##########                 Hardware dependent settings          ##########
##########------------------------------------------------------##########

MCU   = atmega328p
F_CPU = 8000000UL
BAUD  = 256000UL

## Toolchain install directory
TOOLCHAIN_DIR = /Users/brni/dev/lib/avr8-gnu-toolchain-darwin_x86_64/avr/include

## Directory to create the output in
OUT_DIR = build

DOC_DIR = doc

##########------------------------------------------------------##########
##########                  Program Locations                   ##########
##########------------------------------------------------------##########

CC = avr-gcc
AVRAR = avr-ar
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size

##########------------------------------------------------------##########
##########            Target, sources, objects, headers         ##########
##########------------------------------------------------------##########

TARGET = build/Module.a

COMMON_MODULE_DISPATCHER = ../communication/dispatcher_module
COMMON_MODULE_KILOBOT = ../communication/module_kilobot
COMMON = ../communication

SOURCES  = $(filter-out bootldr.c, $(wildcard *.c))
SOURCES += $(wildcard $(COMMON_MODULE_KILOBOT)/*.c)
SOURCES += $(wildcard $(COMMON_MODULE_DISPATCHER)/*.c)
OBJECTS  = $(SOURCES:.c=.o)
HEADERS  = $(wildcard *.h)

ASM_FILES_DIR = ../communication/module_kilobot

##########------------------------------------------------------##########
##########                          Flags                       ##########
##########------------------------------------------------------##########

CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I. -I$(TOOLCHAIN_DIR) -I$(COMMON) -I$(COMMON_MODULE_DISPATCHER) -I$(COMMON_MODULE_KILOBOT)
CPPFLAGS_ASSEMBLY = -x assembler-with-cpp -gdwarf-2 -std=gnu99

CFLAGS = -O1 -g -std=gnu99 -Wall
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections

LDFLAGS += -Wl,--start-group  -Wl,--end-group -Wl,--gc-sections

TARGET_ARCH = -mmcu=$(MCU)

##########------------------------------------------------------##########
##########                          Rules                       ##########
##########------------------------------------------------------##########

$(TARGET): $(OBJECTS) $(ASM_FILES_DIR)/message_send.o
	$(AVRAR) rcs $@ $(OBJECTS) $(ASM_FILES_DIR)/message_send.o
	rm -f $(OBJECTS)

##  To make .o files from .c files
%.o: %.c $(HEADERS) Makefile
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<

## Compile the assembly source
$(ASM_FILES_DIR)/message_send.o: $(ASM_FILES_DIR)/message_send.S $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CPPFLAGS_ASSEMBLY) $(TARGET_ARCH) -c -o $@ $<

$(OUT_DIR)/module_bootloader.elf: bootldr.o $(TARGET)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) -o $@ $^

$(OUT_DIR)/module_bootloader.hex: $(OUT_DIR)/module_bootloader.elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures $< $@
	rm -f $(OBJECTS)
	$(AVRSIZE) $(AVR_SIZE_FLAGS) $<
	rm $<


## These targets don't have files named after them
.PHONY: all doc

directories: ${OUT_DIR}

doc:
	${MAKE} -C ${DOC_DIR}

${OUT_DIR}:
	mkdir $@

all: directories $(TARGET)

bootloader: directories $(OUT_DIR)/module_bootloader.hex

clean:
	rm -rf build
	rm -f $(OBJECTS)
	rm -f bootldr.o
