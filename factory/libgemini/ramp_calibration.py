# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

import argparse
import json
import math
import os.path
import pathlib
import statistics
import time

import pyvisa as visa
from wintertools import keyboard, log, oscilloscope, tui

from libgemini import gemini, oscillators, reference_calibration

here = os.path.abspath(os.path.dirname(__file__))
period_to_dac_code = reference_calibration.castor.copy()
start_color = (1.0, 1.0, 0.0)
end_color = (0.5, 0.6, 1.0)

AVERAGE_COUNT = 2


# Use max because PK-PK has poor resolution and also includes negative transients.
def _measure_max(scope, scope_channel):
    return statistics.mean(
        scope.get_max(scope_channel) for _ in range(0, AVERAGE_COUNT)
    )


def _set_code_and_measure_max(gem, scope, dac_channel, scope_channel, code):
    gem.set_dac(dac_channel, code, vref=0)
    time.sleep(0.005)
    return _measure_max(scope, scope_channel)


def _seek_voltage_on_channel(
    gem, scope, oscillator, period, start_code, target_voltage
):
    output = tui.Updateable(clear_all=True)
    frequency = oscillators.timer_period_to_frequency(period)
    low_threshold, high_threshold = target_voltage - 0.15, target_voltage + 0.05
    low_code, high_code = 0, 0

    if oscillator == 0:
        dac_channel = 0
        scope_channel = "c1"
    else:
        dac_channel = 2
        scope_channel = "c2"

    gem.set_period(oscillator, period)
    gem.set_dac(dac_channel, start_code, vref=0)

    # Let things settle.
    time.sleep(0.01)

    # Phase one: wait for frequency to be within range.
    with output:
        while True:
            measured_frequency = scope.get_frequency()

            freq_diff = abs(measured_frequency / frequency) - 1.0

            if freq_diff > 0.3:
                print(
                    f"Waiting for {frequency}, currently measuring {measured_frequency}"
                )
                output.update()
                time.sleep(0.2)
                continue

            break

    # Phase two: find initial high and low values.
    initial_pkpk = _measure_max(scope, scope_channel)

    # Initial measure is within range, nothing to do.
    if initial_pkpk >= low_threshold and initial_pkpk <= high_threshold:
        return start_code

    # Start code is our new *low* code, find the high code.
    if initial_pkpk < low_threshold:
        low_code = start_code

        # start at the low code and go up.
        high_code = low_code

        with output:
            while True:
                # To improve seek time, move exponentially.
                high_code = math.ceil(high_code * 1.1)

                if high_code >= 4095:
                    high_code = 4095
                    break

                pkpk = _set_code_and_measure_max(
                    gem, scope, dac_channel, scope_channel, high_code
                )

                if pkpk > high_threshold:
                    break

                print(f"Seeking high code, currently at {high_code}")
                output.update()

    # Start code is our new *high* value, find the low value.
    elif initial_pkpk > high_threshold:
        high_code = start_code

        # start at the high code and go down
        low_code = high_code

        with output:
            while True:
                low_code = math.floor(low_code * 0.90)

                if low_code <= 1:
                    low_code = 1
                    break

                pkpk = _set_code_and_measure_max(
                    gem, scope, dac_channel, scope_channel, low_code
                )

                if pkpk < low_threshold:
                    break

                print(f"Seeking low code, currently at {low_code}")
                output.update()

    # Phase three: binary search
    while True:
        code = low_code + math.floor(((high_code - low_code) / 2))
        print(f"Trying {code}, low: {low_code}, high: {high_code}... ", end="")

        pkpk = _set_code_and_measure_max(gem, scope, dac_channel, scope_channel, code)

        print(f"measured {pkpk}.")

        # We've exhausted the search.
        if code == low_code:
            # TODO: fall back to one-by-one method?
            if code == 4095:
                return 4095

            print("Binary search exhausted, re-trying...")
            low_code = math.floor(low_code * 0.90)
            high_code = min(math.floor(high_code * 1.10), 4095)
            continue

        if pkpk > high_threshold:
            # if setting the high code would just repeat the same code next
            # iteration, nudge it.
            if code + 1 == high_code:
                high_code = min(high_code + 4, 4095)
            else:
                high_code = min(code + 1, 4095)
            continue

        elif pkpk < low_threshold:
            if code - 1 == low_code:
                low_code = max(low_code - 4, 0)
            else:
                low_code = max(code - 1, 0)
            continue

        else:
            print(f"Binary search found {code} for {pkpk:.2f} peak-to-peak")
            return code

    raise RuntimeError("I shouldn't have gotten here...")


def _calibrate_oscillator(gem, scope, oscillator):
    bar = tui.Bar()
    output = tui.Updateable()

    if oscillator == 0:
        scope_channel = "c1"
        scope.enable_channel("c1")
        scope.disable_channel("c2")
        dac_channel = 0
    else:
        scope_channel = "c2"
        scope.enable_channel("c2")
        scope.disable_channel("c1")
        dac_channel = 2

    scope.set_trigger_level(scope_channel, 1)
    scope.show_measurement(scope_channel, "PKPK")
    scope.show_measurement(scope_channel, "MAX")
    scope.set_time_division("10ms")

    # Wait a moment for the scope to get ready.
    time.sleep(0.2)

    # Start with the lowest note and manually calibrate it. This will be used
    # as the reference for other notes.
    lowest_period = list(period_to_dac_code.keys())[0]
    start_code = period_to_dac_code[lowest_period]
    gem.set_period(oscillator, lowest_period)
    time.sleep(0.2)

    print(
        "Calibrate first note: use up and down to change DAC code, press enter to accept."
    )

    with output:
        while True:
            gem.set_dac(dac_channel, start_code, 0)
            print(
                f"DAC code: {start_code}, voltage: {oscillators.charge_code_to_volts(start_code)}"
            )
            output.update()
            key = keyboard.read()
            if key == keyboard.UP:
                start_code += 1
            elif key == keyboard.DOWN:
                start_code -= 1
            elif key == keyboard.ENTER:
                break

    period_to_dac_code[lowest_period] = start_code

    target_voltage = _measure_max(scope, scope_channel)

    log.info(
        f"Calibrated {oscillators.timer_period_to_frequency(lowest_period)} to {start_code} with magnitude of {target_voltage}."
    )

    last_dac_code = start_code

    for n, (period, dac_code) in enumerate(period_to_dac_code.items()):
        progress = n / (len(period_to_dac_code) - 1)

        if dac_code < last_dac_code:
            dac_code = last_dac_code

        # Adjust the oscilloscope's time division as needed.
        frequency = oscillators.timer_period_to_frequency(period)

        if frequency > 500:
            scope.set_time_division("250us")
        elif frequency > 300:
            scope.set_time_division("500us")
        elif frequency > 150:
            scope.set_time_division("1ms")
        elif frequency > 50:
            scope.set_time_division("5ms")
        else:
            scope.set_time_division("10ms")

        bar.draw(
            tui.Segment(progress, color=tui.gradient(start_color, end_color, progress)),
        )
        print(f"Frequency: {frequency:.2f} Hz, Period: {period}")

        period_to_dac_code[period] = last_dac_code = _seek_voltage_on_channel(
            gem, scope, oscillator, period, dac_code, target_voltage
        )

    return period_to_dac_code.copy()


def run(save):
    # Oscilloscope setup.
    log.info("Configuring oscilloscope...")
    resource_manager = visa.ResourceManager("@ivi")
    scope = oscilloscope.Oscilloscope(resource_manager)
    scope.reset()

    scope.enable_bandwidth_limit()
    scope.set_time_division("10ms")
    scope.set_vertical_cursor("c1", 0, 3.3)
    scope.set_vertical_division("c1", "800mV")
    scope.set_vertical_division("c2", "800mV")
    scope.set_vertical_offset("c1", -1.65)
    scope.set_vertical_offset("c2", -1.65)

    # Gemini setup
    log.info("Connecting to Gemini...")
    gem = gemini.Gemini()
    gem.enter_calibration_mode()

    input(
        "Connect PROBE ONE to RAMP A\nConnect PROBE TWO to RAMP B\n> press enter to start."
    )

    # Calibrate both oscillators
    log.section("Calibrating Pollux...", depth=2)
    pollux_calibration = _calibrate_oscillator(gem, scope, 1)

    lowest_voltage = oscillators.charge_code_to_volts(min(pollux_calibration.values()))
    highest_voltage = oscillators.charge_code_to_volts(max(pollux_calibration.values()))
    log.success(
        f"\nCalibrated:\n- Lowest: {lowest_voltage:.2f}v\n- Highest: {highest_voltage:.2f}v\n"
    )

    log.section("Calibrating Castor...", depth=2)
    castor_calibration = _calibrate_oscillator(gem, scope, 0)

    lowest_voltage = oscillators.charge_code_to_volts(min(castor_calibration.values()))
    highest_voltage = oscillators.charge_code_to_volts(max(castor_calibration.values()))
    log.success(
        f"\nCalibrated:\n- Lowest: {lowest_voltage:.2f}v\n- Highest: {highest_voltage:.2f}v\n"
    )

    log.section("Saving calibration table...", depth=2)

    local_copy = pathlib.Path("calibrations") / f"{gem.serial_number}.ramp.json"
    local_copy.parent.mkdir(parents=True, exist_ok=True)

    with local_copy.open("w") as fh:
        data = {
            "castor": castor_calibration,
            "pollux": pollux_calibration,
        }
        json.dump(data, fh)

    log.success(f"Saved local copy to {local_copy}")

    if save:
        output = tui.Updateable()
        bar = tui.Bar()

        log.info("Sending LUT values to device...")

        for o, table in enumerate([castor_calibration, pollux_calibration]):
            for n, dac_code in enumerate(table.values()):
                with output:
                    progress = n / (len(table.values()) - 1)
                    bar.draw(
                        tui.Segment(
                            progress,
                            color=tui.gradient(start_color, end_color, progress),
                        ),
                    )
                    log.debug(f"Set oscillator {o} entry {n} to {dac_code}.")

                    gem.write_lut_entry(n, o, dac_code)

        log.info("Committing LUT to NVM...")
        gem.write_lut()

        checksum = 0
        for dac_code in castor_calibration.values():
            checksum ^= dac_code

        log.success(f"Calibration table written, checksum: {checksum:04x}")

    else:
        log.warning("Dry run enabled, calibration table not saved to device.")

    gem.close()

    print("")
    log.success("Done!")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument(
        "--dry_run",
        action="store_true",
        default=False,
        help="Don't save the calibration values.",
    )

    args = parser.parse_args()

    run(not args.dry_run)
