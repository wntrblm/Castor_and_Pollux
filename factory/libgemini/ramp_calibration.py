# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

import json
import os.path
import pathlib
import time

from wintertools import interactive, oscilloscope, reportcard, tui
from wintertools.print import print

from libgemini import gemini, oscillators, reference_calibration

here = os.path.abspath(os.path.dirname(__file__))
period_to_dac_code = reference_calibration.castor.copy()
start_color = (1.0, 1.0, 0.0)
end_color = (0.5, 0.6, 1.0)


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


def _calibrate_oscillator(gem, scope, oscillator):
    bar = tui.Bar()

    if oscillator == 0:
        scope_channel = "c1"
        scope.enable_channel("c1")
        scope.disable_channel("c2")
    else:
        scope_channel = "c2"
        scope.enable_channel("c2")
        scope.disable_channel("c1")

    scope.set_trigger_level(scope_channel, "1.65V")
    scope.set_cursor_type("Y")
    scope.set_vertical_cursor(scope_channel, "-3.3V", "0V")
    scope.set_vertical_division(scope_channel, "800mV")
    scope.set_vertical_offset(scope_channel, "-1.65V")
    scope.show_measurement(scope_channel, "PKPK")
    scope.show_measurement(scope_channel, "MAX")

    scope.set_time_division("10ms")

    # Wait a moment for the scope to get ready.
    time.sleep(0.2)

    last_dac_code = 0

    for n, (period, dac_code) in enumerate(period_to_dac_code.items()):
        progress = n / (len(period_to_dac_code) - 1)

        if dac_code < last_dac_code:
            dac_code = last_dac_code

        frequency = oscillators.timer_period_to_frequency(period)
        scope.set_time_division_from_frequency(frequency)

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
    scope = oscilloscope.Oscilloscope()
    gem = gemini.Gemini.get()
    gem.enter_calibration_mode()
    time.sleep(0.1)

    initial_period, initial_dac_code = next(iter(period_to_dac_code.items()))
    gem.set_frequency(0, oscillators.timer_period_to_frequency(initial_period))
    gem.set_frequency(1, oscillators.timer_period_to_frequency(initial_period))
    gem.set_dac(initial_dac_code, 2048, initial_dac_code, 2048)

    # scope.reset()
    scope.enable_bandwidth_limit()
    scope.set_intensity(50, 100)

    scope.set_time_division("10ms")
    scope.enable_channel("c1")
    scope.set_vertical_division("c1", "800mV")
    scope.set_vertical_offset("c1", "-1.65V")
    scope.enable_channel("c2")
    scope.set_vertical_division("c2", "800mV")
    scope.set_vertical_offset("c2", "-1.65V")

    print("!! Confirm sawtooth waveforms are visible before continuing!")
    interactive.continue_when_ready()

    # Calibrate both oscillators
    print("Calibrating Castor...")
    castor_calibration = _calibrate_oscillator(gem, scope, 0)

    print("Calibrating Pollux...")
    pollux_calibration = _calibrate_oscillator(gem, scope, 1)

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
        print("checksum: {checksum:04x}")

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
