## These targets don't have files named after them
.PHONY: all doc directories clean fuses flash debug

##########------------------------------------------------------##########
##########                 Hardware dependent settings          ##########
##########------------------------------------------------------##########

MCU   = atmega328p
F_CPU = 8000000UL

## Toolchain install directory
TOOLCHAIN_DIR = /Users/brni/dev/lib/avr8-gnu-toolchain-darwin_x86_64/avr/include

## Directory to create the output and documentation
OUT_DIR = build
DOC_DIR = doc

## Programmer
PROGRAMMER_ARGS = -p m328p -P usb -c avrispmkII
FUSES = -U lfuse:w:0xff:m -U hfuse:w:0xd1:m

##########------------------------------------------------------##########
##########                  Program Locations                   ##########
##########------------------------------------------------------##########

CC = avr-gcc
AVRAR = avr-ar
AVROC = avr-objcopy
AVRDUDE = avrdude
AVRSIZE = avr-size


##########------------------------------------------------------##########
##########            Target, sources, objects, headers         ##########
##########------------------------------------------------------##########

TARGET = build/Dispatcher

COMMON_MODULE = ../communication/dispatcher_module
COMMON_KILOGUI = ../communication/kilogui_dispatcher
COMMON = ../communication


SOURCES = $(wildcard *.c)
SOURCES += $(wildcard $(COMMON_MODULE)/*.c)

OBJECTS = $(SOURCES:.c=.o)
HEADERS = $(wildcard *.h)

##########------------------------------------------------------##########
##########                          Flags                       ##########
##########------------------------------------------------------##########


CPPFLAGS = -DF_CPU=$(F_CPU) -I. -I$(TOOLCHAIN_DIR) -I$(COMMON_MODULE) -I$(COMMON_KILOGUI) -I$(COMMON)
CPPFLAGS_ASSEMBLY = -x assembler-with-cpp -gdwarf-2 -std=gnu99 

CFLAGS = -O1 -g -std=gnu99 -Wall
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections

LDFLAGS += -Wl,--start-group  -Wl,--end-group -Wl,--gc-sections

TARGET_ARCH = -mmcu=$(MCU)

AVR_SIZE_FLAGS = -C --mcu=$(MCU)

##########------------------------------------------------------##########
##########                          Rules                       ##########
##########------------------------------------------------------##########

##  To make .o files from .c files
%.o: %.c $(HEADERS) Makefile
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<

$(TARGET).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -o $@ $(OBJECTS)

$(TARGET).hex: $(TARGET).elf
	$(AVROC) -O ihex $(FLASH) $< $@
	rm -f $(OBJECTS)
	$(AVRSIZE) $(AVR_SIZE_FLAGS) $(TARGET).elf
	rm $(TARGET).elf

##########------------------------------------------------------##########
##########                          Flash                       ##########
##########------------------------------------------------------##########

fuses:
	$(AVRDUDE) $(PROGRAMMER_ARGS) $(FUSES)

flash: $(TARGET).hex 
	$(AVRDUDE) $(PROGRAMMER_ARGS) -U flash:w:$<:i

##########------------------------------------------------------##########
##########                          Misc                        ##########
##########------------------------------------------------------##########

debug:
	@echo
	@echo "Source files:"   $(SOURCES)
	@echo "Header files:"	$(HEADERS)
	@echo "Object files:"	$(OBJECTS)
	@echo "MCU, F_CPU, BAUD:"  $(MCU), $(F_CPU), $(BAUD)
	@echo

directories: ${OUT_DIR}

doc:
	${MAKE} -C ${DOC_DIR}

${OUT_DIR}:
	mkdir $@

all: directories $(TARGET).hex

clean:
	rm -rf build
	rm -f $(OBJECTS)
