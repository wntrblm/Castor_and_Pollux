# Interface to Sol over MIDI


class Sol:
    MIDI_PORT_NAME = "Sol usb_midi.ports[0]"

    def __init__(self):
        self.port_out, _ = rtmidi.midiutil.open_midiport(self.MIDI_PORT_NAME, type_="output")

    def send_note(self, number):
        self.port_out.send_message([0x90, number, 0x7F])