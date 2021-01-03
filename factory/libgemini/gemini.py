# Interface for Gemini's MIDI SysEx command set

from dataclasses import dataclass
import enum
import struct

import rtmidi.midiutil

from libgemini import teeth

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
        return int(x * 65536.0 + 0.5)
    else:
        return int(x * 65536.0 - 0.5)


@dataclass
class Settings:
    LENGTH: int = 51

    magic: int = 0
    adc_gain_corr: int = 0
    adc_offset_corr: int = 0
    led_brightness: int = 0
    castor_knob_min: int = 0
    castor_knob_max: int = 0
    pollux_knob_min: int = 0
    pollux_knob_max: int = 0
    chorus_max_intensity: int = 0
    chorus_frequency: int = 0
    knob_offset_corr: int = 0
    knob_gain_corr: int = 0
    smooth_initial_gain: int = 0
    smooth_sensitivity: int = 0
    pollux_follower_threshold: int = 0
    castor_lfo_pwm: bool = False
    pollux_lfo_pwm: bool = False


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


class Gemini:
    MIDI_PORT_NAME = "Gemini"

    def __init__(self):
        self.port_in, _ = rtmidi.midiutil.open_midiport(self.MIDI_PORT_NAME, type_="input")
        self.port_in.ignore_types(sysex=False)
        self.port_out, _ = rtmidi.midiutil.open_midiport(self.MIDI_PORT_NAME, type_="output")

    def close(self):
        self.port_in.close_port()
        self.port_out.close_port()

    def _sysex(self, command, data=None, response=False, encode=False, decode=False):
        if data is None:
            data = []

        if encode:
            data = teeth.teeth_encode(data)
        
        self.port_out.send_message(
            [SYSEX_START, SYSEX_MARKER, command] + data + [SYSEX_END])
        
        if response:
            result = _wait_for_message(self.port_in)

            if decode:
                return teeth.decode(result[3:-1])
            
            return result

    def enter_calibration_mode(self):
        resp = self._sysex(SysExCommands.HELLO, response=True)
        print(f"Gemini version: {resp[3]}")

    def read_adc(self, ch):
        resp = self._sysex(SysExCommands.READ_ADC, data=[ch], response=True, decode=True)
        val, = struct.unpack("H", resp)
        return val

    def set_dac(self, ch, val, gain):
        data = struct.pack("BBH", ch, gain, val)
        self._sysex(SysExCommands.SET_DAC, data=data, encode=True)

    def set_period(self, ch, val):
        data = struct.pack("BI", ch, val)
        self._sysex(SysExCommands.SET_FREQ, data=data, encode=True)

    def set_adc_gain_error_int(self, val):
        data = struct.pack("H", val)
        self._sysex(SysExCommands.WRITE_ADC_GAIN, data=data, encode=True)
    
    def set_adc_gain_error(self, val):
        val = int(val * 2048)
        self.set_adc_gain_error_int(val)

    def set_adc_offset_error(self, val):
        data = struct.pack("H", val)
        self._sysex(SysExCommands.WRITE_ADC_OFFSET, data=data, encode=True)

    def disable_adc_error_correction(self):
        self._sysex(SysExCommands.DISABLE_ADC_CORR)

    def enable_adc_error_correction(self):
        self._sysex(SysExCommands.ENABLE_ADC_CORR)

    def reset_settings(self):
        self._sysex(SysExCommands.RESET_SETTINGS)

    SETTINGS_CHUNKS = 4
    CHUNK_SIZE = 20

    def read_settings(self):
        settings_encoded = bytearray(teeth.teeth_encoded_length(self.CHUNK_SIZE * self.SETTINGS_CHUNKS))

        for n in range(self.SETTINGS_CHUNKS):
            data = self._sysex(SysExCommands.READ_SETTINGS, data=[n], response=True, decode=True)
            settings_encoded[self.CHUNK_SIZE * n: self.CHUNK_SIZE * n + self.CHUNK_SIZE] = data

        settings_buf = teeth.teeth_decode(settings_encoded)
        settings = Settings()

        (settings.magic,
        settings.adc_gain_corr,
        settings.adc_offset_corr,
        settings.led_brightness,
        settings.castor_knob_min,
        settings.castor_knob_max,
        settings.pollux_knob_min,
        settings.pollux_knob_max,
        settings.chorus_max_intensity,
        settings.chorus_frequency,
        settings.knob_offset_corr,
        settings.knob_gain_corr,
        settings.smooth_initial_gain,
        settings.smooth_sensitivity,
        settings.pollux_follower_threshold,
        settings.castor_lfo_pwm,
        settings.pollux_lfo_pwm) = struct.unpack(">BHhHiiiiiiiiiiH??", settings_buf[:Settings.LENGTH])

        return settings

    def save_settings(self, settings):
        settings_buf = bytearray(struct.pack(
            ">BHhHiiiiiiiiiiH??",
            settings.magic,
            settings.adc_gain_corr,
            settings.adc_offset_corr,
            settings.led_brightness,
            settings.castor_knob_min,
            settings.castor_knob_max,
            settings.pollux_knob_min,
            settings.pollux_knob_max,
            settings.chorus_max_intensity,
            settings.chorus_frequency,
            settings.knob_offset_corr,
            settings.knob_gain_corr,
            settings.smooth_initial_gain,
            settings.smooth_sensitivity,
            settings.pollux_follower_threshold,
            settings.castor_lfo_pwm,
            settings.pollux_lfo_pwm))

        settings_encoded = teeth.teeth_encode(settings_buf).ljust(self.CHUNK_SIZE * self.SETTINGS_CHUNKS)

        for n in range(self.SETTINGS_CHUNKS):
            chunk = settings_encoded[self.CHUNK_SIZE * n: self.CHUNK_SIZE * n + self.CHUNK_SIZE]
            self._sysex(SysExCommands.WRITE_SETTINGS, data=chunk, response=True)

    def write_lut_entry(self, entry, castor_pollux, val):
        data = struct.pack("BBH", entry, castor_pollux, val)
        self._sysex(SysExCommands.WRITE_LUT_ENTRY, data=data, encode=True, response=True)

    def write_lut(self):
        self._sysex(SysExCommands.WRITE_LUT)

    def erase_lut(self):
        self._sysex(SysExCommands.ERASE_LUT)