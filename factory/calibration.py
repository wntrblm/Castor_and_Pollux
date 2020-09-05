import rtmidi
import rtmidi.midiutil

MIDI_PORT_NAME = "TinyUSB Device"
SYSEX_START = 0xF0
SYSEX_END = 0xF7

midiout = rtmidi.MidiOut()
port_in, _ = rtmidi.midiutil.open_midiport(MIDI_PORT_NAME, type_="input")
port_out, _ = rtmidi.midiutil.open_midiport(MIDI_PORT_NAME, type_="output")


def set_dac(ch, val):
    port_out.send_message([SYSEX_START, 0x77, 0x06, ch, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])


def set_period(ch, val):
    port_out.send_message([SYSEX_START, 0x77, 0x07, ch, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])


periods = [
901,
850,
803,
757,
715,
675,
637,
601,
567,
535,
505,
477,
]

dac_codes = [0] * len(periods)

starting_dac_code = 954

for n, period in enumerate(periods):
    print(f"Period: {period}")
    set_period(0, period)
    dac_code = starting_dac_code

    while True:
        set_dac(0, dac_code)
        reply = input("(u)p, (d)own, (u)p a (l)ittle, (d)own a (l), or (o)kay)? ")
        if reply == "u":
            dac_code += 10
        if reply == "d":
            dac_code -= 10
        if reply == "ul":
            dac_code += 1
        if reply == "dl":
            dac_code -= 1
        if reply == "o":
            dac_codes[n] = dac_code
            break

    starting_dac_code = dac_code


for dac_code in dac_codes:
    print(f"{dac_code}")