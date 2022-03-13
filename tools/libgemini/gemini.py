# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

# Interface for Gemini's MIDI SysEx command set

import enum
import statistics
import struct

from wintertools import midi, teeth

from libgemini import gem_settings


class SysExCommands(enum.IntEnum):
    # Only directly expose "user-facing" commands, ie. ones which are likely to be useful
    # to non-factory code.
    HELLO = 0x01
    #WRITE_ADC_GAIN = 0x02
    #WRITE_ADC_OFFSET = 0x03
    #READ_ADC = 0x04
    #SET_DAC = 0x05
    #SET_FREQ = 0x06
    RESET_SETTINGS = 0x07
    READ_SETTINGS = 0x18
    WRITE_SETTINGS = 0x19
    #WRITE_LUT_ENTRY = 0x0A
    #WRITE_LUT = 0x0B
    #ERASE_LUT = 0x0C
    #DISABLE_ADC_CORR = 0x0D
    #ENABLE_ADC_CORR = 0x0E
    GET_SERIAL_NUMBER = 0x0F
    #MONITOR = 0x10
    SOFT_RESET = 0x11
    #ENTER_CALIBRATION = 0x12
    RESET_INTO_BOOTLOADER = 0x13


class Gemini(midi.MIDIDevice):
    MIDI_PORT_NAME = "Gemini"
    SYSEX_MARKER = 0x77

    def __init__(self):
        super().__init__()
        self.version = None
        self.serial_number = None

    def get_firmware_version(self):
        resp = self.sysex(SysExCommands.HELLO, response=True)
        self.version = bytearray(resp[3:-1]).decode("ascii")
        return self.version

    def get_serial_number(self):
        resp = self.sysex(SysExCommands.GET_SERIAL_NUMBER, response=True, decode=True)
        self.serial_number = resp.hex()
        return self.serial_number

    def reset_settings(self):
        self.sysex(SysExCommands.RESET_SETTINGS)

    def read_settings(self):
        settings_buf = self.sysex(
            SysExCommands.READ_SETTINGS, response=True, decode=True
        )
        settings = gem_settings.GemSettings.unpack(settings_buf)
        return settings

    def save_settings(self, settings):
        settings_buf = settings.pack()
        self.sysex(
            SysExCommands.WRITE_SETTINGS, settings_buf, encode=True, response=True
        )

    def soft_reset(self):
        self.sysex(SysExCommands.SOFT_RESET)

    def reset_into_bootloader(self):
        self.sysex(SysExCommands.RESET_INTO_BOOTLOADER)
