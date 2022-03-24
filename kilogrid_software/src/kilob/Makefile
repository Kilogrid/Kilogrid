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

##########------------------------------------------------------##########
##########            Target, sources, objects, headers         ##########
##########------------------------------------------------------##########

TARGET = build/kilob.a

COMMON_MODULE_KILOBOT = ../communication/module_kilobot
COMMON = ../communication

SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(wildcard *.h)

KILOLIB_DIR = ../kilolib_v2

##########------------------------------------------------------##########
##########                          Flags                       ##########
##########------------------------------------------------------##########

CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I. -I$(TOOLCHAIN_DIR) -I$(COMMON) -I$(COMMON_MODULE_KILOBOT) -I$(KILOLIB_DIR)
CPPFLAGS_ASSEMBLY = -x assembler-with-cpp -gdwarf-2 -std=gnu99 

CFLAGS = -O1 -g -std=gnu99 -Wall
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections 

LDFLAGS += -Wl,--start-group  -Wl,--end-group -Wl,--gc-sections

TARGET_ARCH = -mmcu=$(MCU)

##########------------------------------------------------------##########
##########                          Rules                       ##########
##########------------------------------------------------------##########

$(TARGET): $(OBJECTS)
	$(AVRAR) rcs $@ $(OBJECTS)
	rm -f $(OBJECTS)

##  To make .o files from .c files
%.o: %.c $(HEADERS) Makefile
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<

## These targets don't have files named after them
.PHONY: all doc bootldr

directories: ${OUT_DIR}

doc:
	${MAKE} -C ${DOC_DIR}

${OUT_DIR}:
	mkdir $@

all: directories $(TARGET)

clean:
	rm -rf build
	rm -f $(OBJECTS)
