# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

"""Ramp calibration values estimated based on circuit design. These values
will get close to the target ramp voltage, but will probably be a little off.
Generally, reference_calibration is used instead."""


def _frequency_to_timer_period(frequency):
    return round(((8_000_000 / 1) / frequency) - 1)


def _midi_note_to_frequency(note):
    return pow(2, (note - 69) / 12) * 440


def _estimate_charge_code(frequency):
    return min(round(10 * frequency / 5_000 / 3.3 * 4095), 4095)


castor = {
    _frequency_to_timer_period(_midi_note_to_frequency(note)): _estimate_charge_code(
        _midi_note_to_frequency(note)
    )
    for note in range(12, 97)
}

pollux = castor.copy()
