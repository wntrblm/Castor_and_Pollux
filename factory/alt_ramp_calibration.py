# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

import json
import os.path
import pathlib
import time

#from wintertools import interactive, oscilloscope, reportcard, tui
from wintertools import interactive, reportcard, tui
from wintertools.print import print

from libgemini import gemini, oscillators, reference_calibration

here = os.path.abspath(os.path.dirname(__file__))
period_to_dac_code = reference_calibration.castor.copy()
start_color = (1.0, 1.0, 0.0)
end_color = (0.5, 0.6, 1.0)

def get_firmware_and_serial():
    print("# Firmware & serial")

    gem = gemini.Gemini.get()
    fw_version = gem.get_firmware_version()
    serial = gem.get_serial_number()

    print(f"Firmware version: {fw_version}")
    print(f"Serial number: {serial}")

    REPORT.ulid = serial
    REPORT.sections.append(
        reportcard.Section(
            name="Firmware",
            items=[
                reportcard.LabelValueItem(
                    label="Version", value=fw_version, class_="stack"
                ),
                reportcard.LabelValueItem(
                    label="Serial number", value=serial, class_="stack"
                ),
            ],
        )
    )

def _manual_seek(gem, charge_code):
    output = tui.Updateable(persist=False)
    adjuster = interactive.adjust_value(charge_code, min=0, max=4095)

    time.sleep(0.1)

    with output:
        for value in adjuster:
            charge_code = value
            gem.set_dac(charge_code, 2048, charge_code, 2048)
            print(
                f"code: {charge_code}, voltage: {oscillators.charge_code_to_volts(charge_code):03f}"
            )
            output.update()

    return charge_code


def _calibrate_oscillator(gem, oscillator):
    bar = tui.Bar()

    # Wait a moment for the scope to get ready.
    time.sleep(0.2)

    last_dac_code = 0

    for n, (period, dac_code) in enumerate(period_to_dac_code.items()):
        progress = n / (len(period_to_dac_code) - 1)

        if dac_code < last_dac_code:
            dac_code = last_dac_code

        frequency = oscillators.timer_period_to_frequency(period)
        print(f"Frequency: {frequency}")
        #scope.set_time_division_from_frequency(frequency)

        output = tui.Updateable(persist=False)

        with output:
            bar.draw(
                tui.Segment(
                    progress, color=tui.gradient(start_color, end_color, progress)
                ),
            )

            print(f"Calibrating ramp for {frequency=:.2f} Hz")

            # If we've measured more than twice, we have enough info to determine
            # the slope of the charge voltage - it should be pretty much linear, so
            # we can guess a code very close to the right one.
            if n > 2:
                x_1, y_1 = list(period_to_dac_code.items())[n - 1]
                x_2, y_2 = list(period_to_dac_code.items())[0]
                x_1 = oscillators.timer_period_to_frequency(x_1)
                x_2 = oscillators.timer_period_to_frequency(x_2)
                slope = (y_2 - y_1) / (x_2 - x_1)
                y_intercept = y_2 - (slope * x_2)
                dac_code = min(4095, round(y_intercept + (slope * frequency)))
                print(
                    f"[italic]Guessed DAC code as {dac_code} from slope {slope:02f}[/]"
                )

            gem.set_frequency(oscillator, frequency)

            if dac_code < 4095:
                calibrated_code = _manual_seek(gem, dac_code)
            else:
                calibrated_code = 4095

        period_to_dac_code[period] = calibrated_code

        last_dac_code = calibrated_code

    results = period_to_dac_code.copy()

    lowest_voltage = oscillators.charge_code_to_volts(min(results.values()))
    highest_voltage = oscillators.charge_code_to_volts(max(results.values()))

    print("✓ Calibrated:")
    print(f"- lowest: {lowest_voltage:.2f} volts")
    print(f"- Highest: {highest_voltage:.2f} volts")

    return results


def run(save):
    #scope = oscilloscope.Oscilloscope()
    gem = gemini.Gemini.get()
    gem.enter_calibration_mode()
    time.sleep(0.1)

    initial_period, initial_dac_code = next(iter(period_to_dac_code.items()))
    gem.set_frequency(0, oscillators.timer_period_to_frequency(initial_period))
    gem.set_frequency(1, oscillators.timer_period_to_frequency(initial_period))
    gem.set_dac(initial_dac_code, 2048, initial_dac_code, 2048)

    print("!! Confirm sawtooth waveforms are visible before continuing!")
    interactive.continue_when_ready()

    # Calibrate both oscillators
    print("Set the scope Horizontal Time Base to 10ms and the Vertical Scale to 1V.")
    print("You will need to adjust the Horizontal Time Base as the frequency increases")
    input("\nConnect a scope to the Castor output (Left hand side) then press Enter to continue...")
    castor_calibration = _calibrate_oscillator(gem, 0)
    #castor_calibration = _calibrate_oscillator(gem, scope, 0)

    input("\nConnect a scope to the Pollux output (Right hand side), then press Enter to continue...")
    pollux_calibration = _calibrate_oscillator(gem, 1)
    #pollux_calibration = _calibrate_oscillator(gem, scope, 1)

    local_copy = pathlib.Path("calibrations") / f"{gem.serial_number}.ramp.json"
    local_copy.parent.mkdir(parents=True, exist_ok=True)

    with local_copy.open("w") as fh:
        data = {
            "castor": castor_calibration,
            "pollux": pollux_calibration,
        }
        json.dump(data, fh)

    print(f"[italic]Saved ramp table to {local_copy}[/]")

    if save:
        print("Sending ramp table values to device...")

        checksum = 0
        for n, timer_period in enumerate(castor_calibration.keys()):
            castor_code = castor_calibration[timer_period]
            pollux_code = pollux_calibration[timer_period]
            checksum ^= castor_code
            gem.write_lut_entry(n, timer_period, castor_code, pollux_code)

        gem.write_lut()

        print("✓ Saved ramp table to NVM")
        print(f"checksum: {checksum:04x}")

    else:
        print("[italic]Dry run enabled, ramp table not saved to device.[/]")

    report_graph = reportcard.LineGraph(
        height=500,
        x_axis=reportcard.Axis(
            label="Freq (Hz)",
            min=0,
            max=2_000,
            min_label="0",
            max_label="2k",
            ease=reportcard.Ease.cube,
        ),
        y_axis=reportcard.Axis(
            label="Charge (V)",
            min=0,
            max=3.3,
            min_label="0",
            max_label="3.3",
        ),
        grid_lines=reportcard.GridLines(
            x_step=0.1,
            y_step=0.5,
        ),
    )

    # Pass validation if the charge code didn't hit the ceiling before the
    # later 1/4 of the frequency range.
    calibration_len = len(castor_calibration)
    castor_passed = all(
        x < 4095
        for x in list(castor_calibration.values())[: int(calibration_len * 3 / 4)]
    )
    pollux_passed = all(
        x < 4095
        for x in list(pollux_calibration.values())[: int(calibration_len * 3 / 4)]
    )

    print("Performing a soft reset....")
    gem.soft_reset()

    return reportcard.Section(
        name="Sawtooth",
        items=[
            reportcard.PassFailItem(
                label="Castor ramp calibration", value=castor_passed
            ),
            reportcard.PassFailItem(
                label="Pollux ramp calibration", value=pollux_passed
            ),
            reportcard.LineGraphItem(
                series=[
                    reportcard.Series(
                        data=[
                            (
                                oscillators.timer_period_to_frequency(period),
                                oscillators.charge_code_to_volts(code),
                            )
                            for period, code in castor_calibration.items()
                        ]
                    ),
                    reportcard.Series(
                        data=[
                            (
                                oscillators.timer_period_to_frequency(period),
                                oscillators.charge_code_to_volts(code),
                            )
                            for period, code in pollux_calibration.items()
                        ],
                    ),
                ],
                graph=report_graph,
            ),
        ],
    )


if __name__ == "__main__":

    print()
    print("> This script calibrates ramp amplitude at different frequencies")
    print("When asked to adjust the waveform, the aim is to have the highest amplitude without the waveform showing a flat section at the top")
    print()
    print("!! Please confirm the following are true, then press ENTER to continue:")
    print("* This machine connected to the main board USB port")
    print("* There is not a drive visible named GEMINIBOOT. If so, please power cycle the main board")
    print("* The SAW output is set to full CW and all other waveforms to full CCW on both Castor and Pollux")
    print("* The main board is connected to eurorack power")
    input()

    REPORT = reportcard.Report(name="Castor & Pollux")
    get_firmware_and_serial()
    REPORT.sections.append(run(save=True))
    print(REPORT)
