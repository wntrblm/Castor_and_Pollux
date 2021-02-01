# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

"""Helpers for calculating various aspects of the oscillator's behavior."""

TCC_GCLK_FREQUENCY = 8_000_000
TCC_GCLK_DIVISION = 1
TCC_GCLK_RESOLUTION = 2 ** 24
ADC_RESOLUTION = 2 ** 12
ADC_SCALE = 6.0


def midi_note_range():
    return range(12, 97)


def midi_note_to_voltage(note):
    return (note - 12) * (1 / 12)


def midi_note_to_frequency(note):
    return pow(2, (note - 69) / 12) * 440


def frequency_to_timer_period(frequency):
    return round(((TCC_GCLK_FREQUENCY / TCC_GCLK_DIVISION) / frequency) - 1)


def timer_period_to_frequency(period):
    return TCC_GCLK_FREQUENCY / (TCC_GCLK_DIVISION * (period + 1))


def voltage_to_adc_code(voltage):
    return round((ADC_SCALE - voltage) / ADC_SCALE * ADC_RESOLUTION)


def charge_code_for_frequency(frequency):
    return min(round(10 * frequency / 5_000 / 3.3 * 4095), 4095)


def charge_code_to_volts(charge_code):
    return charge_code / 4095 * 3.3
