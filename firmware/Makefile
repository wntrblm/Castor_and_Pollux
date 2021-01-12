# This file shouldn't usually be edited.
# Change settings in boardconfig.mk.

include mcuconfig.mk
include boardconfig.mk

# Toolchain configuration
BUILD = build
BIN = gemini-firmware
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

ifeq ($(OS), Windows_NT)
  MKDIR = gmkdir
else
  MKDIR = mkdir
endif

# Common compiler & linker flags.

# Set the mcu parameters. Cortex-M CPUs only support the Thumb instruction set.
COMMON_FLAGS += -mthumb -mcpu=$(MCU_MCPU) -mfloat-abi=$(MCU_FLOAT_ABI) -mfpu=$(MCU_FPU)

# Compiler configuration

CFLAGS += $(COMMON_FLAGS)
# Use C11 and nanolib/newlib-nano
CFLAGS += --std=gnu11 --specs=nano.specs
CFLAGS += -W -Wall -Wextra -Werror -Wshadow -Wdouble-promotion -Wformat=2
# Stack usage warning, must be the same as in the linker script.
CFLAGS += -Wstack-usage=$(shell printf "%d" $(STACK_SIZE))
# Specify dialect options that make things easier for embedded development.
CFLAGS += -funsigned-char -funsigned-bitfields -fshort-enums
# Specify EABI dialects - aapcs configures variable size enums, since that's what
# the GCC toolchain seems to be compiled with. aapcs-linux defines enums as
# 4 bytes.
CFLAGS += -mabi=aapcs
# -MD: output make *.d files
# -MP: This option instructs CPP to add a phony target for each dependency other
# than the main file, causing each to depend on nothing. These dummy rules work around
# errors make gives if you remove header files without updating the Makefile to match
CFLAGS += -MD -MP -MT $(BUILD)/$(*F).o -MF $(BUILD)/$(@F).d

# Optimizations
# Enable dead code elimination
CFLAGS += -fdata-sections -ffunction-sections

# Optimize for debuggability.
DEBUGCFLAGS += -ggdb3 -Og

# Optimize for speed instead of size, as this firmware is very small.
RELEASECFLAGS += -O2

INCLUDES += \
  -I./src \
  -I./src/hw \
  -I./src/drivers \
  -I./src/lib \
  -I./third_party/samd21/include \
  -I./third_party/cmsis/include

INCLUDES += -I$(MCU_CMSIS_INCLUDE)

SRCS += $(wildcard src/*.c) $(wildcard src/**/*.c)

# Include the MCU startup script
SRCS += $(MCU_STARTUP)

# Configure defines for the hardware/CMSIS libraries
DEFINES += \
  -DDONT_USE_CMSIS_INIT \
  -DF_CPU=$(F_CPU) \
	-D__$(MCU)__ \
	-D$(MCU_MATH)

DEFINES += $(MCU_DEFINES)

# Third-party libraries


# Segger RTT
INCLUDES += \
    -I./third_party/rtt
SRCS += \
    ./third_party/rtt/SEGGER_RTT.c

# Printf
DEFINES += -DPRINTF_DISABLE_SUPPORT_FLOAT -DPRINTF_DISABLE_SUPPORT_EXPONENTIAL
INCLUDES += -I./third_party/printf
SRCS += ./third_party/printf/printf.c

# TinyUSB
INCLUDES += -I./third_party/tinyusb/src
SRCS += \
  ./third_party/tinyusb/src/tusb.c \
  ./third_party/tinyusb/src/class/midi/midi_device.c \
  ./third_party/tinyusb/src/common/tusb_fifo.c \
  ./third_party/tinyusb/src/device/usbd.c \
  ./third_party/tinyusb/src/device/usbd_control.c \
  ./third_party/tinyusb/src/portable/microchip/samd/dcd_samd.c
DEFINES += -DCFG_TUSB_MCU=$(TINYUSB_MCU)

# Libfixmath
INCLUDES += -I./third_party/libfixmath
SRCS += $(wildcard third_party/libfixmath/*.c)
DEFINES += -DFIXMATH_FAST_SIN -DFIXMATH_NO_CACHE

# Structy
INCLUDES += -I./third_party/structy
SRCS += $(wildcard third_party/structy/*.c)


# Combine compiler configuration into CFLAGS

CFLAGS += $(INCLUDES) $(DEFINES)

# Linker configuration

LDFLAGS += $(COMMON_FLAGS)
# Set the stack size symbol for the linker script
LDFLAGS += -Xlinker --defsym=__stack_size__=$(STACK_SIZE)
# Perform dead code elimination 
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--script=$(LINKER_SCRIPT)
# Generate a map file
LDFLAGS += -Wl,-Map=build/link.map

LIBS += -lm

##############################################################################

OBJS = $(addprefix $(BUILD)/, $(notdir %/$(subst .c,.o, $(SRCS))))

# Rules

.PHONY: all directory clean size firmware release debug format tidy

all: debug

debug: DEFINES += -DDEBUG
debug: CFLAGS += $(DEBUGCFLAGS)
debug: directory firmware size

release: DEFINES += -DNDEBUG
release: CFLAGS += $(RELEASECFLAGS)
release: clean directory firmware size

$(BUILD)/$(BIN).elf: $(OBJS)
	@echo LD $@
	@$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

$(BUILD)/$(BIN).hex: $(BUILD)/$(BIN).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -O ihex $^ $@

$(BUILD)/$(BIN).bin: $(BUILD)/$(BIN).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -O binary $^ $@

$(BUILD)/$(BIN).uf2: $(BUILD)/$(BIN).bin
	@echo uf2conv $@
	@python3 third_party/uf2/uf2conv.py -b 0x2000 -o $@ --convert $(BUILD)/$(BIN).bin

%.o:
	@echo CC $@
	@$(CC) $(CFLAGS) $(filter %/$(subst .o,.c,$(notdir $@)), $(SRCS)) -c -o $@

firmware: directory $(BUILD)/$(BIN).elf $(BUILD)/$(BIN).bin $(BUILD)/$(BIN).uf2
	@echo "Created: \n  $(BUILD)/$(BIN).elf\n  $(BUILD)/$(BIN).bin\n  $(BUILD)/$(BIN).uf2"

directory:
	@$(MKDIR) -p $(BUILD)

./src/gem_voice_param_table.c: ./data/pitch-calibration-table.csv
	@echo "Generating gem_voice_param_table.c"
	@./scripts/generate_voice_param_table.py data/pitch-calibration-table.csv src/gem_voice_param_table.c

size: $(BUILD)/$(BIN).elf
	@echo ""
	@./scripts/get_fw_size.py --flash-size $(FLASH_SIZE) --ram-size $(RAM_SIZE) $^ 

clean:
	@echo clean
	@-rm -rf $(BUILD)

format:
	clang-format -i src/**.h src/**.c

tidy:
	clang-tidy src/**.c -checks=-clang-diagnostic-format -- $(INCLUDES) $(DEFINES)

-include $(wildcard $(BUILD)/*.d)

