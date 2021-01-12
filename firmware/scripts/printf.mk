# Adds /third_party/printf - embedded-friendly printf

INCLUDES += -I./third_party/printf
SRCS += ./third_party/printf/printf.c
DEFINES += -DPRINTF_DISABLE_SUPPORT_FLOAT -DPRINTF_DISABLE_SUPPORT_EXPONENTIAL
