#!/usr/bin/env python3

# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

import math

from libgemini.gemini import Gemini
from libgemini.gem_quantizer import GemQuantizerConfig

GEMINI_MAX_CV = 7 # Volts

### Example scales

# Build a scale based on equally divided octaves
# Special case: 12 notes per octave gives the standard Western 12-tone equal tempered scale
# If `descending` is true, then the scale is reversed so that higher input CVs lead to
# lower pitches. This is mainly for testing, but may be useful for other purposes.
def build_edo_scale(notes_per_octave=12, descending=False):
    num_notes = int(math.floor(GEMINI_MAX_CV * notes_per_octave + 1))
    volts_per_note = 1. / notes_per_octave

    scale = []
    for note in range(num_notes):
        note_voltage = note * volts_per_note
        threshold = note_voltage - (volts_per_note / 2.)
        if descending:
            scale.append((threshold, GEMINI_MAX_CV - note_voltage))
        else:
            scale.append((threshold, note_voltage))

    return GemQuantizerConfig(scale=scale)

def read_scale(gemini):
    print("Reading scale...")
    quantizer = gemini.read_quantizer()
    print("Hysteresis:", quantizer.hysteresis)
    print("Num notes:", len(quantizer.scale))
    print("First note:", repr(quantizer.scale[0]))
    print("Last note:", repr(quantizer.scale[-1]))

def write_scale(gemini, scale):
    print("Writing scale")
    gemini.save_quantizer(scale)

def main():
    print("Connecting...")
    gemini = Gemini()
    print("Connected")
    if 0:
        scale = build_edo_scale(12)
        write_scale(gemini, scale)
        read_scale(gemini)
    elif 0:
        read_scale(gemini)
    else:
        print("Reading version...")
        verstr = gemini.get_firmware_version()
        print(verstr)
        print("Reading settings...")
        settings = gemini.read_settings()
        print(settings)

if __name__ == "__main__":
    main()
