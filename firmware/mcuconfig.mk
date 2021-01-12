# Flags passed in the compiler. This should include -mcpu and the floating
# point configuration.
# - cortex-m0plus
# - cortex-m4
# - cortex-m7
MCU_MCPU = cortex-m0plus

# Floating point configuration.
# See https://embeddedartistry.com/blog/2017/10/11/demystifying-arm-floating-point-compiler-options/

# Must be soft, softfp, or hard.
# For Cortex M0+ this should be soft.
# For Cortex M4F and M7F this should be hard.
MCU_FLOAT_ABI = soft

# Floating point unit type.
# Cortex M0+: auto (emulated)
# Cortex M4F: fpv4-sp-d16
# Cortex M7: fpv5-sp-d16
MCU_FPU = auto

# CMSIS ARM math definition.
# See https://github.com/ARM-software/CMSIS/blob/master/CMSIS/Include/arm_math.h#L84-L88
# Cortex M0+: ARM_MATH_CM0PLUS
# Cortex M4F: ARM_MATH_CM4
# Cortex M7: ARM_MATH_CM7
MCU_MATH = ARM_MATH_CM0PLUS

# CMSIS include headers from the CMSIS support package
# for this MPU.
MCU_CMSIS_INCLUDE = ./third_party/samd21/include

# Startup script location for this CPU. This is generally
# from the CMSIS support package as well.
MCU_STARTUP = ./third_party/samd21/gcc/gcc/startup_samd21.c

# A list of extra preprocessor defines for this MCU.
MCU_DEFINES = -DSAMD21

# TinyUSB configuration
TINYUSB_MCU = OPT_MCU_SAMD21
