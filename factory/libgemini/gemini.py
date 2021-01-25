# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

# Interface for Gemini's MIDI SysEx command set

import enum
import struct

import rtmidi.midiutil

from libwinter import teeth, log
from libgemini import gem_settings
from libgemini import gem_monitor_update

SYSEX_START = 0xF0
SYSEX_END = 0xF7
SYSEX_MARKER = 0x77


def _wait_for_message(port_in):
    while True:
        msg = port_in.get_message()
        if msg:
            msg, _ = msg
            return msg


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
    READ_SETTINGS = 0x08
    WRITE_SETTINGS = 0x09
    WRITE_LUT_ENTRY = 0x0A
    WRITE_LUT = 0x0B
    ERASE_LUT = 0x0C
    DISABLE_ADC_CORR = 0x0D
    ENABLE_ADC_CORR = 0x0E
    GET_SERIAL_NUMBER = 0x0F
    MONITOR = 0x10


class Gemini:
    MIDI_PORT_NAME = "Gemini"

    def __init__(self):
        self.port_in, _ = rtmidi.midiutil.open_midiport(
            self.MIDI_PORT_NAME, type_="input"
        )
        self.port_in.ignore_types(sysex=False)
        self.port_out, _ = rtmidi.midiutil.open_midiport(
            self.MIDI_PORT_NAME, type_="output"
        )
        self.version = None
        self.serial_number = None

    def close(self):
        self.port_in.close_port()
        self.port_out.close_port()

    def _sysex(self, command, data=None, response=False, encode=False, decode=False):
        if data is None:
            data = []

        if encode:
            data = teeth.teeth_encode(data)

        self.port_out.send_message(
            [SYSEX_START, SYSEX_MARKER, command] + list(data) + [SYSEX_END]
        )

        if response:
            result = _wait_for_message(self.port_in)

            if decode:
                return teeth.teeth_decode(result[3:-1])

            return result

    def enter_calibration_mode(self):
        resp = self._sysex(SysExCommands.HELLO, response=True)
        self.version = bytearray(resp[3:-1]).decode("ascii")
        log.info(f"Gemini version: {self.version}")

        resp = self._sysex(SysExCommands.GET_SERIAL_NUMBER, response=True)
        self.serial_number = teeth.teeth_decode(resp[3:-1]).hex()
        log.info(f"Serial number: {self.serial_number}")

    def read_adc(self, ch):
        resp = self._sysex(
            SysExCommands.READ_ADC, data=[ch], response=True, decode=True
        )
        (val,) = struct.unpack(">H", resp)
        return val

    def set_dac(self, ch, val, vref):
        data = struct.pack(">BHB", ch, val, vref)
        self._sysex(SysExCommands.SET_DAC, data=data, encode=True)

    def set_period(self, ch, val):
        data = struct.pack(">BI", ch, val)
        self._sysex(SysExCommands.SET_FREQ, data=data, encode=True)

    def set_adc_gain_error_int(self, val):
        data = struct.pack(">H", val)
        self._sysex(SysExCommands.WRITE_ADC_GAIN, data=data, encode=True)

    def set_adc_gain_error(self, val):
        val = int(val * 2048)
        self.set_adc_gain_error_int(val)

    def set_adc_offset_error(self, val):
        data = struct.pack(">h", val)
        self._sysex(SysExCommands.WRITE_ADC_OFFSET, data=data, encode=True)

    def disable_adc_error_correction(self):
        self._sysex(SysExCommands.DISABLE_ADC_CORR)

    def enable_adc_error_correction(self):
        self._sysex(SysExCommands.ENABLE_ADC_CORR)

    def reset_settings(self):
        self._sysex(SysExCommands.RESET_SETTINGS)

    CHUNK_SIZE = 10
    SETTINGS_ENCODED_LEN = teeth.teeth_encoded_length(
        gem_settings.GemSettings.PACKED_SIZE
    )
    SETTINGS_CHUNKS = SETTINGS_ENCODED_LEN // CHUNK_SIZE

    def read_settings(self):
        settings_encoded = bytearray(self.SETTINGS_ENCODED_LEN)

        for n in range(self.SETTINGS_CHUNKS):
            data = self._sysex(SysExCommands.READ_SETTINGS, data=[n], response=True)
            assert len(data) == self.CHUNK_SIZE + 4
            settings_encoded[
                self.CHUNK_SIZE * n : self.CHUNK_SIZE * n + self.CHUNK_SIZE
            ] = data[3:-1]

        settings_buf = teeth.teeth_decode(settings_encoded)
        settings = gem_settings.GemSettings.unpack(settings_buf)
        return settings

    def save_settings(self, settings):
        settings_buf = settings.pack()

        settings_encoded = teeth.teeth_encode(settings_buf)

        for n in range(self.SETTINGS_CHUNKS):
            chunk = list(
                settings_encoded[
                    self.CHUNK_SIZE * n : self.CHUNK_SIZE * n + self.CHUNK_SIZE
                ]
            )
            self._sysex(SysExCommands.WRITE_SETTINGS, data=[n] + chunk, response=True)

    def write_lut_entry(self, entry, castor_pollux, val):
        data = struct.pack(">BBH", entry, castor_pollux, val)
        self._sysex(
            SysExCommands.WRITE_LUT_ENTRY, data=data, encode=True, response=True
        )

    def write_lut(self):
        self._sysex(SysExCommands.WRITE_LUT)

    def erase_lut(self):
        self._sysex(SysExCommands.ERASE_LUT)

    def enable_monitor(self):
        self._sysex(SysExCommands.MONITOR, data=[1])

    def disable_monitor(self):
        self._sysex(SysExCommands.MONITOR, data=[0])

    def monitor(self):
        self.enable_monitor()
        try:
            result = _wait_for_message(self.port_in)
            decoded = teeth.teeth_decode(result[3:-1])
            unpacked = gem_monitor_update.GemMonitorUpdate.unpack(decoded)
            return unpacked
        except Exception as e:
            self.disable_monitor()
            raise e
