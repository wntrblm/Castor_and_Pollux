# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

import argparse
import os.path
import csv
import time
import sys
import pyvisa as visa

from libgemini import gemini
from libgemini import oscilloscope

here = os.path.abspath(os.path.dirname(__file__))

period_to_dac_code = {}

with open(
    os.path.join(here, "../../firmware/data/pitch-calibration-table.csv"), "r"
) as fh:
    reader = csv.DictReader(fh)

    for row in reader:
        period_to_dac_code[int(row["period reg"])] = int(
            row["castor calibrated dac code"]
        )


def _code_to_volts(code):
    return code / 4096 * 2.5


def _period_reg_to_freq(period):
    return 8_000_000 / (4 * (period + 1))


def _calibrate_oscillator(gem, scope, oscillator):
    last_dac_code = 0

    for period, dac_code in period_to_dac_code.items():
        if last_dac_code > dac_code:
            dac_code = last_dac_code

        frequency = _period_reg_to_freq(period)

        print(
            f"Frequency: {frequency:.2f}Hz, Period reg: {period}, start code: {dac_code}, start voltage: {_code_to_volts(dac_code):.2f}v"
        )

        # Adjust the oscilloscope's time division as needed.

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

        gem.set_period(oscillator, period)

        sys.stdout.write("> ")

        while True:
            gem.set_dac(0 if oscillator == 0 else 2, dac_code, gain=1)

            # Let things settle.
            time.sleep(0.02)

            # Read the oscilloscope's peak-to-peak stats and adjust as needed.
            peak_to_peak = scope.get_peak_to_peak("c1")

            if peak_to_peak < 0.3:
                # Probe probably isn't connected, sleep and try again.
                time.sleep(0.2)
                sys.stdout.write("💤")
                sys.stdout.flush()

            elif peak_to_peak <= 3.25:
                # Too low, increase DAC code.
                dac_code += 5
                sys.stdout.write("+")
                sys.stdout.flush()

                if dac_code >= 4095:
                    print("DAC overflow! Voltage can not be increased from here!")
                    dac_code = 4095

            elif peak_to_peak > 3.35:
                # Too high, decrease the DAC code.
                dac_code -= 5
                sys.stdout.write("-")
                sys.stdout.flush()

                if dac_code < 0:
                    print("DAC underflow!")
                    return

            else:
                sys.stdout.write("✓\r\n")
                measured_frequency = scope.get_frequency()
                print(
                    f"Calibrated to code: {dac_code}, voltage: {_code_to_volts(dac_code):.2f}v, peak-to-peak: {peak_to_peak:.2f}v, measured frequency: {measured_frequency:.2f}Hz"
                )
                break

        period_to_dac_code[period] = dac_code
        last_dac_code = dac_code
        print("")

    return period_to_dac_code.copy()


def run(save):
    # Oscilloscope setup.
    resource_manager = visa.ResourceManager("@ivi")
    scope = oscilloscope.Oscilloscope(resource_manager)
    scope.reset()

    scope.set_vertical_division("c1", 2)
    scope.set_vertical_cursor("c1", 0, 3.3)
    scope.set_trigger_level("c1", 1)

    # Gemini setup
    gem = gemini.Gemini()
    gem.enter_calibration_mode()

    # Calibrate both oscillators
    print("--------- Calibrating Castor ---------")
    input("Connect to RAMP A and press enter to start.")
    castor_calibration = _calibrate_oscillator(gem, scope, 0)

    lowest_voltage = _code_to_volts(min(castor_calibration.values()))
    highest_voltage = _code_to_volts(max(castor_calibration.values()))
    print(f"Lowest voltage: {lowest_voltage:.2f}, Highest: {highest_voltage:.2f}")

    print("--------- Calibrating Pollux ---------")
    input("Connect to RAMP B and press enter to start.")
    pollux_calibration = _calibrate_oscillator(gem, scope, 1)

    lowest_voltage = _code_to_volts(min(castor_calibration.values()))
    highest_voltage = _code_to_volts(max(castor_calibration.values()))
    print(f"Lowest voltage: {lowest_voltage:.2f}, Highest: {highest_voltage:.2f}")

    if save:
        print("--------- Saving calibration table ---------")

        for o, table in enumerate([castor_calibration, pollux_calibration]):
            for n, dac_code in enumerate(table.values()):
                print(f"> Set oscillator {o} entry {n} to {dac_code}.")
                gem.write_lut_entry(n, o, dac_code)

        print("Writing LUT to NVM")
        gem.write_lut()

        checksum = 0
        for dac_code in castor_calibration.values():
            checksum ^= dac_code

        print(f"Calibration table written, checksum: {checksum:04x}")

    else:
        print("WARNING: Dry run enabled, calibration table not saved.")

    gem.close()
    print("Done")


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
