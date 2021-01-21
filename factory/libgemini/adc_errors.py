# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

# Helpers for calculating ADC gain and offset error

import statistics


def calculate_avg_gain_error(expected, measured):
    low = int(len(expected) * 0.15)
    high = int(len(expected) * 0.85)
    iterations = int(len(expected) * 0.25)
    gain_errors = []

    for n in range(iterations):
        gain_errors.append(
            (expected[high] - expected[low]) / (measured[high] - measured[low])
        )
        high -= 1
        low += 1

    return statistics.mean(gain_errors)


def calculate_avg_offset_error(expected, measured, gain_error):
    low = int(len(expected) * 0.15)
    iterations = int(len(expected) * 0.50)
    offset_errors = []

    for n in range(iterations):
        offset_errors.append((measured[low] * gain_error) - expected[low])
        low += 1

    return statistics.mean(offset_errors)


def apply_correction(data, gain_error, offset_error):
    result = []
    for value in data:
        result.append((value - offset_error) * gain_error)

    return result
