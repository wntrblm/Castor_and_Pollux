import rtmidi
import rtmidi.midiutil

MIDI_PORT_NAME = "Gemini"
SYSEX_START = 0xF0
SYSEX_END = 0xF7

midiout = rtmidi.MidiOut()
port_in, _ = rtmidi.midiutil.open_midiport(MIDI_PORT_NAME, type_="input")
port_out, _ = rtmidi.midiutil.open_midiport(MIDI_PORT_NAME, type_="output")


def enter_calibration_mode():
    port_out.send_message([SYSEX_START, 0x77, 0x01, SYSEX_END])

def set_dac(ch, val):
    port_out.send_message([SYSEX_START, 0x77, 0x06, ch, 1, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])


def set_period(ch, val):
    port_out.send_message([SYSEX_START, 0x77, 0x07, ch, (val >> 12) & 0xF, (val >> 8) & 0xF, (val >> 4) & 0xF, val & 0xF, SYSEX_END])


periods = [
    61155,
    57723,
    54483,
    51425,
    48539,
    45814,
    43243,
    40816,
    38525,
    36363,
    34322,
    32395,
    30577,
    28861,
    27241,
    25712,
    24269,
    22907,
    21621,
    20407,
    19262,
    18181,
    17160,
    16197,
    15288,
    14430,
    13620,
    12855,
    12134,
    11453,
    10810,
    10203,
    9630,
    9090,
    8580,
    8098,
    7644,
    7214,
    6809,
    6427,
    6066,
    5726,
    5404,
    5101,
    4815,
    4544,
    4289,
    4049,
    3821,
    3607,
    3404,
    3213,
    3033,
    2862,
    2702,
    2550,
    2407,
    2272,
    2144,
    2024,
    1910,
    1803,
    1702,
    1606,
    1516,
    1431,
    1350,
    1275,
    1203,
    1135,
    1072,
    1011,
    955,
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

starting_dac_codes = [
    40,
    40,
    40,
    40,
    42,
    44,
    46,
    48,
    50,
    53,
    56,
    60,
    62,
    65,
    69,
    73,
    77,
    82,
    87,
    91,
    96,
    103,
    108,
    115,
    122,
    129,
    137,
    146,
    154,
    164,
    173,
    183,
    193,
    203,
    215,
    228,
    241,
    255,
    271,
    287,
    307,
    322,
    342,
    362,
    382,
    404,
    428,
    453,
    479,
    512,
    543,
    576,
    616,
    648,
    688,
    728,
    771,
    821,
    865,
    925,
    975,
    1035,
    1095,
    1155,
    1225,
    1305,
    1385,
    1475,
    1556,
    1646,
    1746,
    1856,
    1966,
    2086,
    2216,
    2350,
    2495,
    2638,
    2801,
    2991,
    3159,
    3359,
    3559,
    3780,
    4010,
]

dac_codes = [0] * len(periods)

enter_calibration_mode()

for n, period in enumerate(periods):
    print(f"Period: {period}")
    set_period(1, period)
    dac_code = starting_dac_codes[n]

    while True:
        set_dac(2, dac_code)
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


for dac_code in dac_codes:
    print(f"{dac_code}")