# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

# Helpers for calculating ADC gain and offset error

import statistics


def calculate(expected_low, expected_high, measured_low, measured_high):
    gain_error = (expected_high - expected_low) / (measured_high - measured_low)
    offset_error = (measured_low * gain_error) - expected_low

    return gain_error, offset_error


def calculate_avg_gain_error(expected, measured):
    low = int(len(expected) * 0.2)
    high = int(len(expected) * 0.8)
    gain_errors = []

    while True:
        gain_errors.append(
            (expected[high] - expected[low]) / (measured[high] - measured[low])
        )
        high -= 1
        low += 1
        if high == low:
            break

    return statistics.mean(gain_errors)


def calculate_avg_offset_error(expected, measured, gain_error):
    low = int(len(expected) * 0.15)
    iterations = int(len(expected) * 0.50)
    offset_errors = []

    for n in range(iterations):
        offset_errors.append((measured[low] * gain_error) - expected[low])
        low += 1

    return statistics.mean(offset_errors)


def calculate_avg_errors(expected, measured):
    gain_error = calculate_avg_gain_error(expected, measured)
    offset_error = calculate_avg_offset_error(expected, measured, gain_error)
    return gain_error, offset_error


def apply_correction(value, gain_error, offset_error):
    return (value - offset_error) * gain_error


def apply_correction_list(data, gain_error, offset_error):
    result = []
    for value in data:
        result.append((value - offset_error) * gain_error)

    return result
