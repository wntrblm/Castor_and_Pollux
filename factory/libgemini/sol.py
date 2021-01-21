# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

"""Interface to Sol over MIDI"""

import struct

import rtmidi.midiutil

SYSEX_START = 0xF0
SYSEX_END = 0xF7
SYSEX_MARKER = 0x78


class Sol:
    MIDI_PORT_NAME = "Sol usb_midi.ports[0]"

    def __init__(self):
        self.port_out, _ = rtmidi.midiutil.open_midiport(
            self.MIDI_PORT_NAME, type_="output"
        )

    def send_note(self, number):
        self.port_out.send_message([0x90, number, 0x7F])

    def send_voltage(self, voltage):
        packed = struct.pack("f", voltage)
        encoded = bytearray(
            [
                (packed[0] >> 4) & 0xF,
                packed[0] & 0xF,
                (packed[1] >> 4) & 0xF,
                packed[1] & 0xF,
                (packed[2] >> 4) & 0xF,
                packed[2] & 0xF,
                (packed[3] >> 4) & 0xF,
                packed[3] & 0xF,
            ]
        )

        self.port_out.send_message(
            [
                SYSEX_START,
                SYSEX_MARKER,
                0x01,
                encoded[0],
                encoded[1],
                encoded[2],
                encoded[3],
                encoded[4],
                encoded[5],
                encoded[6],
                encoded[7],
                SYSEX_END,
            ]
        )
