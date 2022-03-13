# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

# Interface for Gemini's MIDI SysEx command set

import enum
import statistics
import struct

from wintertools import midi, teeth

from libgemini import gem_settings, gem_quantizer

SYSEX_START = 0xF0
SYSEX_END = 0xF7

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
    RESET_QUANTIZER = 0x08
    READ_QUANTIZER = 0x1A
    WRITE_QUANTIZER = 0x1B
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
        resp = self.sysex_traced(SysExCommands.HELLO, response=True)
        self.version = bytearray(resp[3:-1]).decode("ascii")
        return self.version

    def get_serial_number(self):
        resp = self.sysex_traced(SysExCommands.GET_SERIAL_NUMBER, response=True, decode=True)
        self.serial_number = resp.hex()
        return self.serial_number

    def reset_settings(self):
        self.sysex_traced(SysExCommands.RESET_SETTINGS)

    def read_settings(self):
        settings_buf = self.sysex_traced(
            SysExCommands.READ_SETTINGS, response=True, decode=True
        )
        settings = gem_settings.GemSettings.unpack(settings_buf)
        return settings

    def save_settings(self, settings):
        settings_buf = settings.pack()
        self.sysex_traced(
            SysExCommands.WRITE_SETTINGS, settings_buf, encode=True, response=True
        )

    def reset_quantizer(self):
        self.sysex_traced(SysExCommands.RESET_QUANTIZER)

    def read_quantizer(self):
        quantizer_buf = self.sysex_traced(
            SysExCommands.READ_QUANTIZER, response=True, decode=True
        )
        quantizer = gem_quantizer.GemQuantizerConfig.unpack(quantizer_buf)
        return quantizer

    def save_quantizer(self, quantizer):
        quantizer_buf = quantizer.pack()
        self.sysex_traced(
            SysExCommands.WRITE_QUANTIZER, quantizer_buf, encode=True, response=True
        )

    def soft_reset(self):
        self.sysex_traced(SysExCommands.SOFT_RESET)

    def reset_into_bootloader(self):
        self.sysex_traced(SysExCommands.RESET_INTO_BOOTLOADER)

    def sysex_traced(self, command, data=None, response=False, encode=False, decode=False):
        if data is None:
            data = []
        
        print(f"Data len = {len(data)}")

        if encode:
            data = teeth.teeth_encode(data)
            print(f"Encoded len = {len(data)}")

        print("Sending...")
        self.port_out.send_message(
            [SYSEX_START, self.SYSEX_MARKER, command] + list(data) + [SYSEX_END]
        )
        print("Sent")

        if response:
            print("Waiting for reply...")
            result = self.wait_for_message()
            print(f"Got reply of len = {len(result)}")

            if decode:
                return teeth.teeth_decode(result[3:-1])
                print(f"Decoded len = {len(result)}")

            return result

