ifndef CAN_RUN
$(error This script should be run from /firmware using "make tests")
endif

# Toolchain configuration
BUILD = build/test
BIN = test
CC = gcc
OBJCOPY = objcopy
SIZE = size

# Build information
include ./scripts/build_info.mk

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
  -I./src/lib

# Linker configuration

LDFLAGS += $(COMMON_FLAGS)
LIBS += -lm

# Include any makefiles for third-party libraries. These files
# should add to INCLUDES, SOURCES, and DEFINES.

THIRD_PARTY_LIBS = \
	munit
include $(addprefix ./scripts/,$(addsuffix .mk,$(THIRD_PARTY_LIBS)))

##############################################################################

# Sources from the firmware

SRCS += \
	src/lib/gem_midi_core.c \
	src/lib/gem_sysex_dispatcher.c

##############################################################################

# Combine final flags and determine all object files.
DEFINES += -DDEBUG

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
