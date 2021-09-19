# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

# Interface for Gemini's MIDI SysEx command set

import enum
import struct

from wintertools import log, midi, teeth

from libgemini import gem_monitor_update, gem_settings


def _fix16(val):
    if val >= 0:
        return int(val * 65536.0 + 0.5)
    else:
        return int(val * 65536.0 - 0.5)


class SysExCommands(enum.IntEnum):
    HELLO = 0x01
    WRITE_ADC_GAIN = 0x02
    WRITE_ADC_OFFSET = 0x03
    READ_ADC = 0x04
    SET_DAC = 0x05
    SET_FREQ = 0x06
    RESET_SETTINGS = 0x07
    READ_SETTINGS = 0x18
    WRITE_SETTINGS = 0x19
    WRITE_LUT_ENTRY = 0x0A
    WRITE_LUT = 0x0B
    ERASE_LUT = 0x0C
    DISABLE_ADC_CORR = 0x0D
    ENABLE_ADC_CORR = 0x0E
    GET_SERIAL_NUMBER = 0x0F
    MONITOR = 0x10
    SOFT_RESET = 0x11
    ENTER_CALIBRATION = 0x12
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

    def enter_calibration_mode(self):
        self.get_firmware_version()
        log.info(f"Gemini version: {self.version}")
        self.get_serial_number()
        log.info(f"Serial number: {self.serial_number}")

        self.sysex(SysExCommands.ENTER_CALIBRATION)

    def read_adc(self, ch):
        resp = self.sysex(SysExCommands.READ_ADC, data=[ch], response=True, decode=True)
        (val,) = struct.unpack(">H", resp)
        return val

    def set_dac(self, a, b, c, d):
        data = struct.pack(">HHHH", a, b, c, d)
        self.sysex(SysExCommands.SET_DAC, data=data, encode=True)

    def set_period(self, ch, val):
        data = struct.pack(">BI", ch, val)
        self.sysex(SysExCommands.SET_FREQ, data=data, encode=True)

    def set_adc_gain_error_int(self, val):
        data = struct.pack(">H", val)
        self.sysex(SysExCommands.WRITE_ADC_GAIN, data=data, encode=True)

    def set_adc_gain_error(self, val):
        val = int(val * 2048)
        self.set_adc_gain_error_int(val)

    def set_adc_offset_error(self, val):
        data = struct.pack(">h", val)
        self.sysex(SysExCommands.WRITE_ADC_OFFSET, data=data, encode=True)

    def disable_adc_error_correction(self):
        self.sysex(SysExCommands.DISABLE_ADC_CORR)

    def enable_adc_error_correction(self):
        self.sysex(SysExCommands.ENABLE_ADC_CORR)

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

    def write_lut_entry(self, entry, period, castor, pollux):
        data = struct.pack(">BIHH", entry, period, castor, pollux)
        self.sysex(SysExCommands.WRITE_LUT_ENTRY, data=data, encode=True, response=True)

    def write_lut(self):
        self.sysex(SysExCommands.WRITE_LUT)

    def erase_lut(self):
        self.sysex(SysExCommands.ERASE_LUT)

    def enable_monitor(self):
        self.sysex(SysExCommands.MONITOR, data=[1])

    def disable_monitor(self):
        self.sysex(SysExCommands.MONITOR, data=[0])

    def monitor(self):
        try:
            result = self.wait_for_message()
            decoded = teeth.teeth_decode(result[3:-1])
            unpacked = gem_monitor_update.GemMonitorUpdate.unpack(decoded)
            return unpacked
        except Exception as e:
            self.disable_monitor()
            raise e

    def soft_reset(self):
        self.sysex(SysExCommands.SOFT_RESET)

    def reset_into_bootloader(self):
        self.sysex(SysExCommands.RESET_INTO_BOOTLOADER)
