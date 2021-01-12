# Adds /third_party/tinyusb

INCLUDES += -I./third_party/tinyusb/src
SRCS += \
  ./third_party/tinyusb/src/tusb.c \
  ./third_party/tinyusb/src/class/midi/midi_device.c \
  ./third_party/tinyusb/src/common/tusb_fifo.c \
  ./third_party/tinyusb/src/device/usbd.c \
  ./third_party/tinyusb/src/device/usbd_control.c \
  ./third_party/tinyusb/src/portable/microchip/samd/dcd_samd.c
DEFINES += -DCFG_TUSB_MCU=$(TINYUSB_MCU)
