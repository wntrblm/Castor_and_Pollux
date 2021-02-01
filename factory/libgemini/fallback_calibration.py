# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

"""Ramp calibration values estimated based on circuit design. These values
will get close to the target ramp voltage, but will probably be a little off.
Generally, reference_calibration is used instead."""

from libgemini import oscillators

castor = {
    oscillators.frequency_to_timer_period(
        oscillators.midi_note_to_frequency(note)
    ): oscillators.charge_code_for_frequency(oscillators.midi_note_to_frequency(note))
    for note in oscillators.midi_note_range()
}

pollux = castor.copy()
