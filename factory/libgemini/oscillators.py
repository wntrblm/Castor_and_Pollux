# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

"""Helpers for calculating various aspects of the oscillator's behavior."""

TCC_GCLK_FREQUENCY = 8_000_000
TCC_GCLK_DIVISION = 1
TCC_GCLK_RESOLUTION = 2 ** 24
ADC_RESOLUTION = 2 ** 12
ADC_SCALE = 6.0
START_NOTE = 12
END_NOTE = 97
CALIBRATION_NOTE_STEP = 6


def midi_note_range():
    return range(12, 97)


def calibration_note_range():
    notes = list(range(START_NOTE, END_NOTE, CALIBRATION_NOTE_STEP))
    if notes[-1] != END_NOTE:
        notes.append(END_NOTE)

    return notes


def midi_note_to_voltage(note):
    return (note - 12) * (1 / 12)


def midi_note_to_frequency(note):
    return pow(2, (note - 69) / 12) * 440


def voltage_to_frequency(voltage):
    return midi_note_to_frequency(12 + (12 * voltage))


def frequency_to_timer_period(frequency):
    return round(((TCC_GCLK_FREQUENCY / TCC_GCLK_DIVISION) / frequency) - 1)


def timer_period_to_frequency(period):
    return TCC_GCLK_FREQUENCY / (TCC_GCLK_DIVISION * (period + 1))


def voltage_to_adc_code(voltage):
    return round((ADC_SCALE - voltage) / ADC_SCALE * ADC_RESOLUTION)


def charge_code_for_frequency(frequency):
    # This accounts for inefficiencies in the real circuit vs the ideal circuit.
    # It's been determined experimentally. The starting point is 1.0. Lower
    # values cause the voltage to grow slower over time and higher values cause
    # it to grow faster over time.
    slope = 0.925

    # 10v output from the ramp corresponds to 3.3v on ramp pre.
    target_voltage = 10

    # The highest frequency obtainable from the ramp oscillator, derived from
    # the RC constant of the charge circuit.
    max_frequency = 5_000

    # The highest voltage the DAC can output
    max_dac_voltage = 3.3

    # The DAC's resolution
    dac_resolution = 2 ** 12 - 1

    return min(
        round(
            slope
            * target_voltage
            * frequency
            / max_frequency
            / max_dac_voltage
            * dac_resolution
        ),
        dac_resolution,
    )


def charge_code_to_volts(charge_code):
    return charge_code / 4095 * 3.3


def find_nearest_pair(values, target):
    low = values[0]
    high = values[0]

    for value in values:
        if value <= target and value >= low:
            low = value
        if value > target:
            high = value
            break
    else:
        high = low

    return low, high


def normalized_difference(low, high, value):
    return (value - low) / (high - low)


def lerp(start, end, normalized_amount):
    return start + ((end - start) * normalized_amount)


def calibrated_charge_code_for_period(period, table):
    low_period, high_period = find_nearest_pair(
        list(reversed(list(table.keys()))), period
    )
    low_code = table[low_period]
    high_code = table[high_period]

    if low_code == high_code:
        return low_code

    offset = normalized_difference(
        timer_period_to_frequency(low_period),
        timer_period_to_frequency(high_period),
        timer_period_to_frequency(period),
    )
    # print(f"{low_period=}, {period=}, {high_period=}, {offset=}")

    return round(lerp(low_code, high_code, offset))
