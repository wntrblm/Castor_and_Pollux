""" Code that runs on Sol to allow the setup scripts to set voltage outputs for calibration. """

import struct
import usb_midi
import winterbloom_sol.sol
import winterbloom_smolmidi as smolmidi

SYSEX_MARKER = 0x78

outputs = winterbloom_sol.sol.Outputs()
midi_in = smolmidi.MidiIn(usb_midi.ports[0])


while True:
    msg = midi_in.receive()

    if not msg:
        continue
    
    if msg.type != smolmidi.SYSEX:
        continue
    
    msg, _ = midi_in.receive_sysex(64)

    if not msg:
        continue

    if msg[0] != SYSEX_MARKER:
        print("Invalid marker")
        continue

    if msg[1] != 0x01:
        print("Invalid command")
        continue
    
    outputs.led.spin()
    
    decoded = bytearray([
        msg[2] << 4 | msg[3],
        msg[4] << 4 | msg[5],
        msg[6] << 4 | msg[7],
        msg[8] << 4 | msg[9],
    ])

    voltage, = struct.unpack("f", decoded)

    outputs.cv_a = voltage

    print(voltage)
