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

# Build information
include scripts/build_info.mk

# Common compiler & linker flags.

# Set the mcu parameters. Cortex-M CPUs only support the Thumb instruction set.
COMMON_FLAGS += -mthumb -mcpu=$(MCU_MCPU) -mfloat-abi=$(MCU_FLOAT_ABI) -mfpu=$(MCU_FPU)
# Use C11 and nanolib/newlib-nano
COMMON_FLAGS += --std=gnu11 --specs=nano.specs

# Compiler configuration

CFLAGS += $(COMMON_FLAGS)
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


# Linker configuration

LDFLAGS += $(COMMON_FLAGS)
# Set the stack size symbol for the linker script
LDFLAGS += -Wl,--defsym=__stack_size__=$(STACK_SIZE)
# Perform dead code elimination
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--script=$(LINKER_SCRIPT)
# Generate a map file
LDFLAGS += -Wl,-Map=build/link.map

LIBS += -lm

# Include any makefiles for third-party libraries. These files
# should add to INCLUDES, SOURCES, and DEFINES.

include $(addprefix scripts/,$(addsuffix .mk,$(THIRD_PARTY_LIBS)))

##############################################################################

# Combine final flags and determine all object files.

CFLAGS += $(INCLUDES) $(DEFINES)
OBJS = $(addprefix $(BUILD)/, $(notdir %/$(subst .c,.o, $(SRCS))))

##############################################################################

# Build rules

.PHONY: all directory clean size firmware release debug format tidy defines

all: debug

debug: BUILD_TYPE=Debug
debug: DEFINES += -DDEBUG
debug: CFLAGS += $(DEBUGCFLAGS)
debug: directory firmware size

release: BUILD_TYPE=Release
release: DEFINES += -DNDEBUG
release: CFLAGS += $(RELEASECFLAGS)
release: clean directory firmware size

$(BUILD)/$(BIN).elf: $(OBJS)
	@echo Link $@
	@$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

$(BUILD)/$(BIN).hex: $(BUILD)/$(BIN).elf
	@$(OBJCOPY) -O ihex $^ $@

$(BUILD)/$(BIN).bin: $(BUILD)/$(BIN).elf
	@$(OBJCOPY) -O binary $^ $@

$(BUILD)/$(BIN).uf2: $(BUILD)/$(BIN).bin
	@python3 third_party/uf2/uf2conv.py -b 0x2000 -o $@ --convert $(BUILD)/$(BIN).bin > /dev/null

%.o:
	@echo Compile $(filter %/$(subst .o,.c,$(notdir $@)), $(SRCS))
	@$(CC) $(CFLAGS) $(filter %/$(subst .o,.c,$(notdir $@)), $(SRCS)) -c -o $@

firmware: directory $(BUILD)/$(BIN).elf $(BUILD)/$(BIN).bin $(BUILD)/$(BIN).uf2
	@echo $(BUILD_TYPE) build
	@echo "Created: \n  $(BUILD)/$(BIN).elf\n  $(BUILD)/$(BIN).bin\n  $(BUILD)/$(BIN).uf2"

directory:
	@mkdir -p $(BUILD)

./src/gem_voice_param_table.c: ./data/pitch-calibration-table.csv
	@echo "Generating gem_voice_param_table.c"
	@./scripts/generate_voice_param_table.py data/pitch-calibration-table.csv src/gem_voice_param_table.c

./src/gem_settings.c: ./data/gem_settings.structy
	@echo "Generating gem_settings.{c,js,py} using Structy"
	@python3 -m structy_generator -l c ./data/gem_settings.structy src
	@clang-format -i src/gem_settings.{h,c}
	@python3 -m structy_generator -l py ./data/gem_settings.structy ../factory/libgemini
	@python3 -m black ../factory/libgemini/gem_settings.py
	@python3 -m structy_generator -l js ./data/gem_settings.structy ../user_guide/docs/scripts

size: $(BUILD)/$(BIN).elf
	@echo ""
	@./scripts/get_fw_size.py --flash-size $(FLASH_SIZE) --ram-size $(RAM_SIZE) $^

clean:
	@echo clean
	@-rm -rf $(BUILD)

format:
	@echo clang-format
	@clang-format -i src/**.h src/**.c src/*/**.h src/*/**.c

tidy:
	@echo clang-tidy
	@clang-tidy src/**.c src/*/**.c -checks=-clang-diagnostic-format -- $(INCLUDES) $(DEFINES)

defines:
	$(CC) $(CFLAGS) -E -dM src/main.c

-include $(wildcard $(BUILD)/*.d)
