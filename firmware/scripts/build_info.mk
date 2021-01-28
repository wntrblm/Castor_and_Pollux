# Build information - used by gem_build_info.{h,c}

# Looks like arm-none-eabi-gcc 10.2.1
ifneq (, $(shell which $(CC)))
BUILD_INFO_COMPILER = $(CC) $(shell $(CC) -dumpversion)
else
BUILD_INFO_COMPILER = no compiler
endif
# Looks like: 12.24.2020
BUILD_INFO_RELEASE = $(shell git describe --always --tags --abbrev=0)
# Looks like: 12 24 2020
BUILD_INFO_RELEASE_PARTS = $(subst ., ,$(BUILD_INFO_RELEASE))
BUILD_INFO_RELEASE_YEAR = $(word 1,$(BUILD_INFO_RELEASE_PARTS))
BUILD_INFO_RELEASE_MONTH = $(word 2,$(BUILD_INFO_RELEASE_PARTS))
BUILD_INFO_RELEASE_DAY = $(word 3,$(BUILD_INFO_RELEASE_PARTS))
# Looks like: 12.24.2020-46-gb77c425-dirty
BUILD_INFO_REVISION = $(shell git describe --always --tags --dirty)
# Looks like: 20/01/2021 22:34 UTC
BUILD_INFO_DATE = $(shell date -u +"%m/%d/%Y %H:%M UTC")
# Looks like: stargirl@stargirls-mbp.lan
BUILD_INFO_MACHINE = $(shell echo `whoami`@`hostname`)

# The whole enchilada. Looks like:
# 12.24.2020-83-g786ee18-dirty (Debug) on 01/23/2021 18:28 UTC with arm-none-eabi-gcc 10.2.1 stargirl@stargirls-mbp.lan
BUILD_INFO_STRING = $(BUILD_INFO_REVISION) ($(BUILD_TYPE)) on $(BUILD_INFO_DATE) with $(BUILD_INFO_COMPILER) $(BUILD_INFO_MACHINE)

DEFINES +=\
	-DBUILD_INFO_COMPILER="\"$(BUILD_INFO_COMPILER)\"" \
	-DBUILD_INFO_RELEASE="\"$(BUILD_INFO_RELEASE)\"" \
	-DBUILD_INFO_REVISION="\"$(BUILD_INFO_REVISION)\"" \
	-DBUILD_INFO_RELEASE_YEAR="$(BUILD_INFO_RELEASE_YEAR)" \
	-DBUILD_INFO_RELEASE_MONTH="$(BUILD_INFO_RELEASE_MONTH)" \
	-DBUILD_INFO_RELEASE_DAY="$(BUILD_INFO_RELEASE_DAY)" \
	-DBUILD_INFO_DATE="\"$(BUILD_INFO_DATE)\"" \
	-DBUILD_INFO_MACHINE="\"$(BUILD_INFO_MACHINE)\"" \
	-DBUILD_INFO_STRING="\"$(BUILD_INFO_STRING)\""
