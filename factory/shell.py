import rtmidi
import rtmidi.midiutil

MIDI_PORT_NAME = "Gemini"
SYSEX_START = 0xF0
SYSEX_END = 0xF7

data = [0, 1, 2, 3, 4, 5]

midiout = rtmidi.MidiOut()
port_in, _ = rtmidi.midiutil.open_midiport(MIDI_PORT_NAME, type_="input")
port_out, _ = rtmidi.midiutil.open_midiport(MIDI_PORT_NAME, type_="output")

port_in.ignore_types(sysex=False)


def callback(event, data=None):
    msg, _ = event

    value = msg[3] << 16 | msg[4] << 8 | msg[5] << 4 | msg[6]

    print("Value: ", value)


port_in.set_callback(callback)

count = 0

while True:
    cmd, val = input().strip().split()
    val = int(val)

    if cmd == "d":
        port_out.send_message([SYSEX_START, 0x77, 0x06, 0x00, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])

    if cmd == "p":
        port_out.send_message([SYSEX_START, 0x77, 0x06, 0x01, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])

    if cmd == "f":
        port_out.send_message([SYSEX_START, 0x77, 0x07, 0x00, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])
        
    if cmd == "adcgain":
        port_out.send_message([SYSEX_START, 0x77, 0x02, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])
        
    if cmd == "adcerror":
        port_out.send_message([SYSEX_START, 0x77, 0x03, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])
    # while True:
    #     msg = port_in.get_message()
    #     if not msg:
    #         break
    #     print(msg)

# https://github.com/SpotlightKid/python-rtmidi/blob/master/examples/basic/midiin_poll.py