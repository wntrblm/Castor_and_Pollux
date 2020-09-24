# Interface for Gemini's MIDI SysEx command set

import rtmidi.midiutil

SYSEX_START = 0xF0
SYSEX_END = 0xF7
SYSEX_MARKER = 0x77

class Gemini:
    MIDI_PORT_NAME = "Gemini"

    def __init__(self):
        self.port_in, _ = rtmidi.midiutil.open_midiport(self.MIDI_PORT_NAME, type_="input")
        self.port_in.ignore_types(sysex=False)
        self.port_out, _ = rtmidi.midiutil.open_midiport(self.MIDI_PORT_NAME, type_="output")

    def enter_calibration_mode(self):
        self.port_out.send_message([SYSEX_START, SYSEX_MARKER, 0x01, SYSEX_END])

    def read_adc(ch):
        self.port_out.send_message([SYSEX_START, SYSEX_MARKER, 0x05, ch, SYSEX_END])

        while True:
            msg = self.port_in.get_message()
            if msg:
                msg, _ = msg
                val = (msg[3] << 16 | msg[4] << 8 | msg[5] << 4 | msg[6])
                return val

    def set_dac(self, ch, val, gain):
        self.port_out.send_message([SYSEX_START, SYSEX_MARKER, 0x06, ch, gain, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])

    def set_period(self, ch, val):
        self.port_out.send_message([SYSEX_START, SYSEX_MARKER, 0x07, ch, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])

    def set_adc_gain_error(self, val)
        self.port_out.send_message([SYSEX_START, SYSEX_MARKER, 0x02, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])
        
    def set_adc_offset_error(self, val)
        self.port_out.send_message([SYSEX_START, SYSEX_MARKER, 0x03, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])