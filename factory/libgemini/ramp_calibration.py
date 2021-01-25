# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

import argparse
import os.path
import time
import pathlib
import json
import pyvisa as visa
import random

from libgemini import gemini, oscilloscope, reference_calibration
from libwinter import tui, log

here = os.path.abspath(os.path.dirname(__file__))
period_to_dac_code = reference_calibration.castor.copy()
start_color = (1.0, 1.0, 0.0)
end_color = (0.5, 0.6, 1.0)


def _code_to_volts(code):
    return code / 4096 * 2.046


def _period_reg_to_freq(period):
    return 8_000_000 / (1 * (period + 1))


def _seek_voltage_on_channel(output, gem, scope, oscillator, period, start_code):

    if oscillator == 0:
        dac_channel = 0
        scope_channel = "c1"
    else:
        dac_channel = 2
        scope_channel = "c2"

    frequency = _period_reg_to_freq(period)
    dac_code = start_code

    gem.set_period(oscillator, period)

    while True:
        # Set the DAC
        gem.set_dac(dac_channel, dac_code, vref=1)

        # Let things settle.
        time.sleep(0.01)

        # Read the oscilloscope's peak-to-peak stats and adjust as needed.
        measured_frequency = scope.get_frequency()
        peak_to_peak = scope.get_peak_to_peak(scope_channel)
        freq_diff = (measured_frequency / frequency) - 1.0
        freq_diff_color = tui.gradient(
            (0.8, 0.8, 1.0), (1.0, 0.5, 0.5), abs(freq_diff) * 2
        )
        code_diff = dac_code - start_code

        # Draw the UI
        output.write(f"Frequency: {frequency:.2f} Hz, Period: {period}\n")
        output.write(
            f"│ {measured_frequency:0.2f} Hz ({tui.rgb(freq_diff_color)}{freq_diff*100:+.0f}%{tui.reset})\n"
        )
        output.write(f"│ Peak-to-peak: {peak_to_peak:.2f} volts\n")

        # Adjust DAC code if needed.
        if peak_to_peak < 0.3:
            # Probe probably isn't connected, sleep and try again.
            output.write(
                f"│ 💤 No input detected, voltage reading at {peak_to_peak:.2f}v\n",
            )
            output.update()
            time.sleep(0.2)
            continue

        elif peak_to_peak <= 3.25:
            # Too low, increase DAC code.
            # Using random here helps it find the range a bit better.
            dac_code += random.randrange(1, 5)

            output.write(
                f"│ {tui.rgb(0.0, 1.0, 1.0)}{_code_to_volts(dac_code):.2f} volts + Δ({_code_to_volts(code_diff):+.4f}, {code_diff} points){tui.reset}\n"
            )
            output.update()

            if dac_code >= 4095:
                log.error("DAC overflow! Voltage can not be increased from here!")
                dac_code = 4095

        elif peak_to_peak > 3.35:
            # Too high, decrease the DAC code.
            dac_code -= random.randrange(1, 5)

            output.write(
                f"│ {tui.rgb(1.0, 1.0, 0.0)}{_code_to_volts(dac_code):.2f} volts - Δ({_code_to_volts(code_diff):.2f}){tui.reset}\n"
            )
            output.update()

            if dac_code < 0:
                log.error("DAC underflow!")
                return

        else:
            output.write(
                f"│ {tui.rgb(0.5, 1.0, 0.5)}{_code_to_volts(dac_code):.2f} volts ✓ Δ({_code_to_volts(code_diff):.2f}){tui.reset}\n"
            )
            output.update()
            log.debug(
                f"Calibrated to code: {dac_code}, voltage: {_code_to_volts(dac_code):.2f}v, peak-to-peak: {peak_to_peak:.2f}v, measured frequency: {measured_frequency:.2f}Hz\n"
            )
            break

    return dac_code


def _calibrate_oscillator(gem, scope, oscillator):
    output = tui.Updateable()
    bar = tui.Bar()

    last_dac_code = 0

    for n, (period, dac_code) in enumerate(period_to_dac_code.items()):
        progress = n / (len(period_to_dac_code) - 1)

        if last_dac_code > dac_code:
            dac_code = last_dac_code

        if dac_code < 30:
            dac_code = 30

        # Adjust the oscilloscope's time division as needed.
        frequency = _period_reg_to_freq(period)

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

        with output:
            bar.draw(
                output,
                tui.Segment(
                    progress, color=tui.gradient(start_color, end_color, progress)
                ),
            )
            period_to_dac_code[period] = last_dac_code = _seek_voltage_on_channel(
                output, gem, scope, oscillator, period, dac_code
            )

    return period_to_dac_code.copy()


def run(save):
    # Oscilloscope setup.
    log.info("Configuring oscilloscope...")
    resource_manager = visa.ResourceManager("@ivi")
    scope = oscilloscope.Oscilloscope(resource_manager)
    scope.reset()

    scope.set_vertical_division("c1", 2)
    scope.set_vertical_cursor("c1", 0, 3.3)
    scope.set_trigger_level("c1", 1)

    # Gemini setup
    log.info("Connecting to Gemini...")
    gem = gemini.Gemini()
    gem.enter_calibration_mode()

    # Calibrate both oscillators
    log.section("Calibrating Castor...", depth=2)

    input("Connect to RAMP A and press enter to start.")
    castor_calibration = _calibrate_oscillator(gem, scope, 0)

    lowest_voltage = _code_to_volts(min(castor_calibration.values()))
    highest_voltage = _code_to_volts(max(castor_calibration.values()))
    log.success(
        f"\nCalibrated:\n- Lowest: {lowest_voltage:.2f}v\n- Highest: {highest_voltage:.2f}v\n"
    )

    log.section("Calibrating Pollux...", depth=2)

    input("Connect to RAMP B and press enter to start.")
    pollux_calibration = _calibrate_oscillator(gem, scope, 1)

    lowest_voltage = _code_to_volts(min(castor_calibration.values()))
    highest_voltage = _code_to_volts(max(castor_calibration.values()))
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
                        output,
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
