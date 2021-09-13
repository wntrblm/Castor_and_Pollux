# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

import argparse
import json
import pathlib
import statistics
import time

from wintertools import log, sol, tui

from libgemini import adc_errors, gemini


def _color_for_diff(diff):
    return tui.rgb(tui.gradient((0.0, 1.0, 0.0), (1.0, 0.0, 0.0), abs(diff) / 300))


def _measure_range(gem, sol_, strategy, sample_count, calibration_points):
    output = tui.Updateable()
    bar = tui.Bar()
    lowest_diff, highest_diff = 0, 0

    results = {}

    with output:
        for n, (voltage, expected_code) in enumerate(calibration_points.items()):
            progress = (n + 1) / len(calibration_points)
            bar.draw(
                tui.Segment(
                    progress,
                    color=tui.gradient((1.0, 1.0, 1.0), (0.5, 0.5, 1.0), progress),
                )
            )
            log.info(f"{tui.reset}Measuring   {voltage:.3f} volts")
            log.info(f"{tui.reset}expecting:  {expected_code}")

            sol_.set_voltage(voltage, channel=strategy.sol_channel)
            time.sleep(0.2)

            samples = []
            for s in range(sample_count):
                samples.append(gem.read_adc(strategy.channel))

            result = strategy.post_measure(statistics.mean(samples))

            diff = result - expected_code

            if diff < lowest_diff:
                lowest_diff = diff

            if diff > highest_diff:
                highest_diff = diff

            if abs(diff) > 300:
                log.error(
                    f"ADC reading too far out of range. Expected {expected_code}, measured: {result:.1f}, diff: {diff:.1f}"
                )

            log.info(f"{tui.reset}measured:   {result:.1f}")
            log.info(f"{tui.reset}diff:       {_color_for_diff(diff)}{diff:.1f}")
            log.info(
                f"{tui.reset}lowest diff: {_color_for_diff(lowest_diff)}{lowest_diff:.1f}{tui.reset}, highest_diff: {_color_for_diff(highest_diff)}{highest_diff:.1f}{tui.reset}"
            )
            output.update()

            results[voltage] = result

    return results


class DirectADCStrategy:
    channel = 8  # LFO pot
    sol_channel = 0
    range_ = 3.3
    resolution = 2 ** 12
    invert = False

    def setup(self, gem):
        gem.disable_adc_error_correction()
        log.info(
            f"Measuring ADC channel {self.channel} via Sol output {self.sol_channel}"
        )

    def save(self, gem, gain_error, offset_error):
        gem.set_adc_gain_error(gain_error)
        gem.set_adc_offset_error(int(offset_error))

    def file_name(self, gem):
        return f"{gem.serial_number}.adc.json"

    def finish(self, gem):
        gem.enable_adc_error_correction()

    def post_measure(self, value):
        return value


class ThroughAFEStrategy:
    channel = 0  # CV A input
    sol_channel = 1
    range_ = 6.0
    resolution = 2 ** 12
    invert = True

    def __init__(self):
        self._gain_error = None
        self._offset_error = None

    def setup(self, gem):
        gem.enable_adc_error_correction()

    def save(self, gem, gain_error, offset_error):
        settings = gem.read_settings()
        settings.cv_gain_error = self._gain_error = gain_error
        settings.cv_offset_error = self._offset_error = offset_error
        gem.save_settings(settings)

    def post_measure(self, value):
        if self._gain_error is None:
            return value

        return adc_errors.apply_correction(value, self._gain_error, self._offset_error)

    def file_name(self, gem):
        return f"{gem.serial_number}.afe.json"

    def finish(self, gem):
        pass


def run(
    num_calibration_points,
    sample_count,
    strategy,
    save,
):
    if strategy == "adc":
        strategy = DirectADCStrategy()
    elif strategy == "afe":
        strategy = ThroughAFEStrategy()
    else:
        raise ValueError(f"Unknonw strategy {strategy}.")

    # Create the list of calibration points and expected codes.
    voltages = [
        n / num_calibration_points * strategy.range_
        for n in range(num_calibration_points + 1)
    ]

    expected_codes = [
        int(voltages[n] / strategy.range_ * (strategy.resolution - 1))
        for n in range(num_calibration_points + 1)
    ]

    if strategy.invert:
        expected_codes = [strategy.resolution - 1 - code for code in expected_codes]

    calibration_points = dict(zip(voltages, expected_codes))

    gem = gemini.Gemini()
    sol_ = sol.Sol()

    sol_.setup()
    sol_.set_voltage(0, channel=strategy.sol_channel)

    gem.enter_calibration_mode()

    strategy.setup(gem)

    pre_calibration_measurements = _measure_range(
        gem, sol_, strategy, sample_count, calibration_points
    )

    gain_error = adc_errors.calculate_avg_gain_error(
        expected_codes, list(pre_calibration_measurements.values())
    )
    offset_error = adc_errors.calculate_avg_offset_error(
        expected_codes, list(pre_calibration_measurements.values()), gain_error
    )

    log.success(f"Measured gain={gain_error:.3f}, offset={offset_error:.1f}")

    strategy.finish(gem)

    local_copy = pathlib.Path("calibrations") / strategy.file_name(gem)
    local_copy.parent.mkdir(parents=True, exist_ok=True)

    with local_copy.open("w") as fh:
        json.dump({"gain_error": gain_error, "offset_error": offset_error}, fh)

    log.info(f"Saved local copy to {local_copy}")

    if save:
        strategy.save(gem, gain_error, offset_error)
        log.success("Saved to NVM.")
    else:
        log.warning("Dry run, not saving to NVM.")
        return

    # Test out the new calibration

    log.info("Taking measurements with new calibration...")

    gem.enable_adc_error_correction()

    post_calibration_measurements = _measure_range(
        gem, sol_, strategy, sample_count, calibration_points
    )

    gain_error = adc_errors.calculate_avg_gain_error(
        expected_codes, list(post_calibration_measurements.values())
    )
    offset_error = adc_errors.calculate_avg_offset_error(
        expected_codes, list(post_calibration_measurements.values()), gain_error
    )
    log.info(f"Remeasured gain={gain_error:.3f}, offset={offset_error:.1f}")

    sol_.set_voltage(strategy.sol_channel, 0)

    # Save readings for further analysis if needed.
    measurement_file = (
        pathlib.Path("calibrations") / "measurements" / strategy.file_name(gem)
    )
    measurement_file.parent.mkdir(parents=True, exist_ok=True)

    with open(measurement_file, "w") as fh:
        json.dump(
            {
                "points": calibration_points,
                "pre": pre_calibration_measurements,
                "post": post_calibration_measurements,
            },
            fh,
            indent=2,
        )
        log.info(f"Saved measurement data to {measurement_file}")

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
    parser.add_argument("--strategy", choices=["adc", "afe"], required=True)
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
        args.strategy,
        not args.dry_run,
    )
