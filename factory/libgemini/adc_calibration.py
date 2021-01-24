# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

import argparse
import time
import pathlib
import json
import statistics

from libwinter import log
from libgemini import gemini
from libgemini import sol
from libgemini import adc_errors


def run(
    calibration_points,
    sample_count,
    adc_range,
    adc_resolution,
    invert,
    adc_channel,
    save,
):
    voltages = [
        n / calibration_points * adc_range for n in range(calibration_points + 1)
    ]

    expected_codes = [
        int(voltages[n] / adc_range * (adc_resolution - 1))
        for n in range(calibration_points + 1)
    ]

    if invert:
        expected_codes = [adc_resolution - 1 - code for code in expected_codes]

    measured_codes = []

    gem = gemini.Gemini()
    sol_ = sol.Sol()

    gem.enter_calibration_mode()
    gem.disable_adc_error_correction()

    sol_.send_voltage(0)

    for n in range(calibration_points + 1):
        voltage = n / calibration_points * adc_range
        log.info(f"Measuring {voltage:.3f}, expecting {expected_codes[n]}.")
        sol_.send_voltage(voltage)
        time.sleep(0.2)

        samples = []
        for s in range(sample_count):
            samples.append(gem.read_adc(adc_channel))

        result = statistics.mean(samples)

        log.info(f"Got {result:.1f}, diff {result - expected_codes[n]:.1f}")
        measured_codes.append(result)

    gain_error = adc_errors.calculate_avg_gain_error(expected_codes, measured_codes)
    offset_error = adc_errors.calculate_avg_offset_error(
        expected_codes, measured_codes, gain_error
    )
    log.info(f"Measured: Gain: {gain_error:.3f}, Offset: {offset_error:.1f}")

    corrected = adc_errors.apply_correction(measured_codes, gain_error, offset_error)
    corrected_gain_error = adc_errors.calculate_avg_gain_error(
        expected_codes, corrected
    )
    corrected_offset_error = adc_errors.calculate_avg_offset_error(
        expected_codes, corrected, corrected_gain_error
    )
    log.success(
        f"Expected after correction: Gain: {corrected_gain_error:.3f}, Offset: {corrected_offset_error:.1f}"
    )

    local_copy = pathlib.Path("calibrations") / f"{gem.serial_number}.adc.json"
    local_copy.parent.mkdir(parents=True, exist_ok=True)

    with local_copy.open("w") as fh:
        json.dump({"gain_error": gain_error, "offset_error": offset_error}, fh)

    log.info(f"Saved local copy to {local_copy}")

    if save:
        gem.set_adc_gain_error(gain_error)
        gem.set_adc_offset_error(int(offset_error))
        log.success("Saved to NVM.")
    else:
        log.warning("Dry run, not saved to NVM.")

    gem.enable_adc_error_correction()

    # Test out the new calibrated ADC

    log.info("Taking measurements with new calibration.")

    measured_codes = []
    for n in range(calibration_points + 1):
        voltage = n / calibration_points * adc_range
        log.debug(f"Measuring {voltage:.3f}, expecting {expected_codes[n]}.")
        sol_.send_voltage(voltage)
        time.sleep(0.2)

        samples = []
        for s in range(sample_count):
            samples.append(gem.read_adc(adc_channel))

        result = statistics.mean(samples)

        log.debug(f"Got {result:.1f}, diff {result - expected_codes[n]:.1f}")
        measured_codes.append(result)

    gain_error = adc_errors.calculate_avg_gain_error(expected_codes, measured_codes)
    offset_error = adc_errors.calculate_avg_offset_error(
        expected_codes, measured_codes, gain_error
    )
    log.success(
        f"Measured, corrected: Gain: {gain_error:.3f}, Offset: {offset_error:.1f}"
    )

    log.success("Done")
    gem.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument(
        "--calibration_points",
        type=int,
        default=50,
        help="Number of calibration points.",
    )
    parser.add_argument(
        "--sample_count",
        type=int,
        default=128,
        help="Number of samples to take at each calibration point.",
    )
    parser.add_argument(
        "--adc_range", type=float, default=3.3, help="ADC range (in volts)."
    )
    parser.add_argument(
        "--adc_resolution", type=int, default=2 ** 12, help="ADC resolution."
    )
    parser.add_argument(
        "--adc_channel",
        type=int,
        default=0,
        help="Which of Gemini's ADC channels to measure.",
    )
    parser.add_argument(
        "--invert",
        action="store_true",
        default=False,
        help="Indicates that the bottom of the voltage range represents the top of the ADC code range",
    )
    parser.add_argument(
        "--dry_run",
        action="store_true",
        default=False,
        help="Don't save the calibration values.",
    )

    args = parser.parse_args()

    run(
        args.calibration_points,
        args.sample_count,
        args.adc_range,
        args.adc_resolution,
        args.invert,
        args.adc_channel,
        not args.dry_run,
    )
