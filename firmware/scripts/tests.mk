# Toolchain configuration
BUILD = build/test
BIN = test
CC = gcc
OBJCOPY = objcopy
SIZE = SIZE

# Common compiler & linker flags.

# Use C11
COMMON_FLAGS += --std=gnu11

# Compiler configuration

CFLAGS += $(COMMON_FLAGS)
CFLAGS += -W -Wall -Wextra -Werror -Wformat=2
CFLAGS += -funsigned-char
# -MD: output make *.d files
# -MP: This option instructs CPP to add a phony target for each dependency other
# than the main file, causing each to depend on nothing. These dummy rules work around
# errors make gives if you remove header files without updating the Makefile to match
CFLAGS += -MD -MP -MT $(BUILD)/$(*F).o -MF $(BUILD)/$(@F).d

# Optimize for debuggability.
CFLAGS += -ggdb3 -Og

INCLUDES += \
  -I./tests/stubs \
  -I./tests \
  -I./src \
  -I./src/hw \
  -I./src/drivers \
  -I./src/lib \
  -I./src/generated \
  -I./third_party/libwinter \
  -I./third_party/libfixmath \
  -I./third_party/tinyusb/src \
  -I./third_party/cmsis/include \
  -I./third_party/samd21/include \
  -I./third_party/munit

# Linker configuration

LDFLAGS += $(COMMON_FLAGS)
LIBS += -lm

##############################################################################

# Sources from the firmware

SRCS += \
	src/gem_oscillator_outputs.c \
	src/lib/gem_sysex_dispatcher.c \
	src/generated/gem_lookup_tables.c \
	third_party/libwinter/wntr_midi_core.c \
	third_party/libwinter/wntr_assert.c \
	third_party/libfixmath/fix16.c \
	third_party/munit/munit.c

##############################################################################

# Combine final flags and determine all object files.
DEFINES += -DDEBUG -D__SAMD21G18A__

CFLAGS += $(INCLUDES) $(DEFINES)
SRCS += $(wildcard tests/*.c) $(wildcard tests/stubs/*.c)
OBJS = $(addprefix $(BUILD)/, $(notdir %/$(subst .c,.o, $(SRCS))))

##############################################################################

# Build rules

.PHONY: all directory clean run

all: run

run: $(BUILD)/$(BIN)
	@echo Running tests
	@$(BUILD)/$(BIN)

$(BUILD)/$(BIN): directory $(OBJS)
	@echo Link $@
	@$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

%.o:
	@echo Compile $(filter %/$(subst .o,.c,$(notdir $@)), $(SRCS))
	@$(CC) $(CFLAGS) $(filter %/$(subst .o,.c,$(notdir $@)), $(SRCS)) -c -o $@

directory:
	@mkdir -p $(BUILD)

-include $(wildcard $(BUILD)/*.d)
