# TODO: Auto-generate this with `structy`
# For now it is hand-written

from dataclasses import dataclass
from typing import ClassVar, List, Tuple

import structy
import struct

MAX_NOTES = 255 # Needs to match MAX_QUANTIZER_TABLE_SIZE in firmware/src/gem_quantizer.h

def unpack_one(fmt, value):
    return struct.unpack(fmt, value)[0]

FIX16_MIN = -(1 << 31)
FIX16_MAX = (1 << 31) - 1

def clamp(value, lo, hi):
    if value < lo:
        return lo
    elif value > hi:
        return hi
    else:
        return value

def clamp_fix16(value):
    return clamp(value, FIX16_MIN, FIX16_MAX)

# Convert from a Python float to a Gemini-compatible `fix16` integer
# This is a 32-bit format with 16 fractional bits
def float_to_fix16(value):
    value = value * 0x00010000
    value += 0.5 if value >= 0 else -0.5
    return clamp_fix16(int(value))

def fix16_to_float(value):
    return float(value) / float(0x00010000)

@dataclass
class GemQuantizerConfig:
    scale: List[Tuple[float, float]]
    hysteresis: float = 0.005

    def pack(self):
        num_notes = len(self.scale)
        assert num_notes <= MAX_NOTES
        buffer = bytearray()
        buffer.extend(struct.pack(">i", float_to_fix16(self.hysteresis)))
        buffer.extend(struct.pack(">B", num_notes))

        # TODO: Check that thresholds are in ascending order

        for (threshold, output) in self.scale:
            buffer.extend(struct.pack(">i", float_to_fix16(threshold)))
            buffer.extend(struct.pack(">i", float_to_fix16(output)))

        return buffer

    @classmethod
    def unpack(buffer):
        hysteresis = fix16_to_float(unpack_one(">i", buffer[0:4]))
        num_notes = unpack_one(">B", buffer[4])

        buffer = buffer[5:]
        scale = []
        for i in range(num_notes):
            threshold = fix16_to_float(unpack_one(">i", buffer[0:4]))
            output = fix16_to_float(unpack_one(">i", buffer[4:8]))
            scale.append((threshold, output))
            buffer = buffer[8:]

        return scale
