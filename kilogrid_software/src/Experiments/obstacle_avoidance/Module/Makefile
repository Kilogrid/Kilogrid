## These targets don't have files named after them
.PHONY: all size clean build doc make_lib

##########------------------------------------------------------##########
##########                 Hardware dependent settings          ##########
##########------------------------------------------------------##########

MCU   = atmega328p
F_CPU = 8000000UL
BAUD  = 9600UL

## Toolchain install directory
TOOLCHAIN_DIR = /home/marco/avr-libc-2.0.0/include/avr
MODULE_LIB_DIR = ../../../module
MODULE_LIB_LIB = $(MODULE_LIB_DIR)/build/Module.a
KILOB_INCLUDE_DIR = ../../../kilob

## Directory to create the output in
OUT_DIR = build

## Directory to create the documentation in
DOC_DIR = doc

##########------------------------------------------------------##########
##########                  Program Locations                   ##########
##########------------------------------------------------------##########

CC = avr-gcc

OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size

##########------------------------------------------------------##########
##########            Target, sources, objects, headers         ##########
##########------------------------------------------------------##########

TARGET = Module

COMMON_MODULE_DISPATCHER = ../../../communication/dispatcher_module
COMMON_MODULE_KILOBOT = ../../../communication/module_kilobot
COMMON = ../../../communication
COMMON_EXPERIMENT = ../Common

SOURCES = $(wildcard *.c)
SOURCES += $(wildcard $(COMMON_MODULE_DISPATCHER)/*.c)
SOURCES += $(wildcard $(COMMON_MODULE_KILOBOT)/*.c)
OBJECTS = $(SOURCES:.c=.o)
HEADERS = $(wildcard *.h)

##########------------------------------------------------------##########
##########                          Flags                       ##########
##########------------------------------------------------------##########

##-MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"
##-MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"
CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I. -I$(TOOLCHAIN_DIR) -I$(MODULE_LIB_DIR) -I$(COMMON) -I$(COMMON_MODULE_DISPATCHER) -I$(COMMON_MODULE_KILOBOT) -I$(COMMON_EXPERIMENT) -I$(KILOB_INCLUDE_DIR)
CPPFLAGS_ASSEMBLY = -x assembler-with-cpp -gdwarf-2 -std=gnu99 

CFLAGS = -O1 -g -std=gnu99 -Wall -L$(MODULE_LIB_LIB)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections

##LDFLAGS = -Wl,-Map,$(TARGET).map 
LDFLAGS += -Wl,--start-group  -Wl,--end-group -Wl,--gc-sections

TARGET_ARCH = -mmcu=$(MCU)
AVR_SIZE_FLAGS = -C --mcu=$(MCU)

##########------------------------------------------------------##########
##########                          Rules                       ##########
##########------------------------------------------------------##########

##  To make .o files from .c files 
%.o: %.c $(HEADERS) Makefile
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<

${OUT_DIR}/$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@ $(MODULE_LIB_LIB)

${OUT_DIR}/$(TARGET).hex: ${OUT_DIR}/$(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures $< $@
	rm -f $(OBJECTS)
	$(AVRSIZE) $(AVR_SIZE_FLAGS) ${OUT_DIR}/$(TARGET).elf
	rm ${OUT_DIR}/$(TARGET).elf

directories: ${OUT_DIR}

doc:
	${MAKE} -C ${DOC_DIR}

build:
	cd $(MODULE_LIB_DIR) && "${MAKE}" all
	rm -rf build
	mkdir $@

all: build ${OUT_DIR}/$(TARGET).hex

debug:
	@echo
	@echo "Source files:"   $(SOURCES)
	@echo "Header files:"	$(HEADERS)
	@echo "MCU, F_CPU, BAUD:"  $(MCU), $(F_CPU), $(BAUD)
	@echo	

size:  ${OUT_DIR}/$(TARGET).elf
	$(AVRSIZE) -C --mcu=$(MCU) $<

clean:
	rm -rf build
	rm -f $(OBJECTS)
