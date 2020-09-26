# Interface for Gemini's MIDI SysEx command set

import enum
import rtmidi.midiutil

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


class SysExCommands(enum.IntEnum):
    HELLO = 0x01
    WRITE_ADC_GAIN = 0x02
    WRITE_ADC_OFFSET = 0x03
    READ_SETTINGS = 0x04
    WRITE_SETTINGS = 0x05
    RESET_SETTINGS = 0x06
    READ_ADC = 0xA5
    SET_DAC = 0xA6
    SET_FREQ = 0xA7


class Gemini:
    MIDI_PORT_NAME = "Gemini"

    def __init__(self):
        self.port_in, _ = rtmidi.midiutil.open_midiport(self.MIDI_PORT_NAME, type_="input")
        self.port_in.ignore_types(sysex=False)
        self.port_out, _ = rtmidi.midiutil.open_midiport(self.MIDI_PORT_NAME, type_="output")

    def enter_calibration_mode(self):
        self.port_out.send_message([SYSEX_START, SYSEX_MARKER, SysExCommands.HELLO, SYSEX_END])
        msg = _wait_for_message(self.port_in)
        print(f"Gemini version: {msg[3]}")

    def read_adc(self, ch):
        self.port_out.send_message([SYSEX_START, SYSEX_MARKER, SysExCommands.READ_ADC, ch, SYSEX_END])
        msg = _wait_for_message(self.port_in)
        val = (msg[3] << 16 | msg[4] << 8 | msg[5] << 4 | msg[6])
        return val

    def set_dac(self, ch, val, gain):
        self.port_out.send_message([SYSEX_START, SYSEX_MARKER, SysExCommands.SET_DAC, ch, gain, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])

    def set_period(self, ch, val):
        self.port_out.send_message([SYSEX_START, SYSEX_MARKER, SysExCommands.SET_FREQ, ch, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])

    def set_adc_gain_error_int(self, val):
        self.port_out.send_message([SYSEX_START, SYSEX_MARKER, SysExCommands.WRITE_ADC_GAIN, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])
    
    def set_adc_gain_error(self, val):
        val = int(val * 2048)
        self.set_adc_gain_error_int(val)

    def set_adc_offset_error(self, val):
        self.port_out.send_message([SYSEX_START, SYSEX_MARKER, SysExCommands.WRITE_ADC_OFFSET, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])

    def reset_settings(self):
        self.port_out.send_message([SYSEX_START, SYSEX_MARKER, SysExCommands.RESET_SETTINGS, SYSEX_END])