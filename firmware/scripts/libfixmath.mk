# Include /third_party/libfixmath - Fixed-point arithmetic library

INCLUDES += -I./third_party/libfixmath
SRCS += $(wildcard third_party/libfixmath/*.c)
DEFINES += -DFIXMATH_FAST_SIN -DFIXMATH_NO_CACHE
