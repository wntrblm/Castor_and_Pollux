# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

import json
import pathlib
import statistics

from hubble import Hubble
from wintertools import reportcard, tui
from wintertools.print import print

from libgemini import adc_errors, gemini

RESOLUTION = 4096
V_MIN = -0.5
V_MAX = 6.1
V_RANGE = abs(V_MIN) + abs(V_MAX)
CHANNEL = gemini.Gemini.ADC.CV_A
NUM_CALIBRATION_POINTS = 50
SAMPLE_COUNT = 64


def _code_to_volts(code):
    code = RESOLUTION - 1 - code
    return V_MIN + (V_RANGE - ((code / (RESOLUTION - 1)) * V_RANGE))


def _volts_to_code(volts):
    code = (volts - V_MIN) / V_RANGE * (RESOLUTION - 1)
    code = RESOLUTION - 1 - code
    return int(code)


def _format_diff(diff):
    r, g, b = map(
        lambda x: int(x * 255),
        tui.gradient((0.0, 1.0, 0.0), (1.0, 0.0, 0.0), abs(diff) / 100),
    )
    return f"[rgb({r},{g},{b})]{diff:0.1f}[/]"


def run():
    gem = gemini.Gemini.get()
    hubble = Hubble.get()
    report_section = reportcard.Section(name="Pitch CV")

    gem.enter_calibration_mode()
    gem.enable_adc_error_correction()
    hubble.VOUT1B.voltage = 0

    volts_per_step = V_RANGE / NUM_CALIBRATION_POINTS

    calibration_points = {
        V_MIN + (n * volts_per_step): _volts_to_code(V_MIN + (n * volts_per_step))
        for n in range(NUM_CALIBRATION_POINTS + 1)
    }

    pre_measurements, pre_gain_error, pre_offset_error = _measure_range(
        gem, hubble, calibration_points
    )

    print(f"Measured gain={pre_gain_error:.3f}, offset={pre_offset_error:.1f}")

    settings = gem.read_settings()
    settings.cv_gain_error = pre_gain_error
    settings.cv_offset_error = pre_offset_error
    gem.save_settings(settings)
    print("✓ Saved to device NVM.")

    print("Taking measurements with new calibration...")

    post_measurements, post_gain_error, post_offset_error = _measure_range(
        gem,
        hubble,
        calibration_points,
        post_measure=lambda x: adc_errors.apply_correction(
            x, pre_gain_error, pre_offset_error
        ),
    )

    print(
        f"[bold]Adjusted gain={post_gain_error:.3f}, offset={post_offset_error:.1f}[/]"
    )

    hubble.VOUT1B.voltage = 0

    report_data = []
    for voltage, expected in calibration_points.items():
        measured = post_measurements[voltage]
        diff = expected - measured
        diff_in_volts = _code_to_volts(diff)
        diff_in_cents = diff_in_volts / (1 / 12) * 100
        report_data.append((voltage, diff_in_cents))

    passed = (0.99 < post_gain_error < 1.01) and (-5 < post_offset_error < 5)
    report_section.items.extend(
        [
            reportcard.PassFailItem(label="Calibration", value=passed),
            reportcard.LabelValueItem(
                label="Gain error", value=f"{pre_gain_error:0.3f}"
            ),
            reportcard.LabelValueItem(
                label="Offset error", value=f"{pre_offset_error:+0.1f}"
            ),
            reportcard.LabelValueItem(
                label="Adj gain error", value=f"{post_gain_error:0.3f}"
            ),
            reportcard.LabelValueItem(
                label="Adj offset error", value=f"{post_offset_error:+0.1f}"
            ),
            reportcard.LineGraphItem(
                series=reportcard.Series(data=report_data),
                graph=reportcard.LineGraph(
                    height=500,
                    x_axis=reportcard.Axis(
                        label="Input (V)",
                        min=V_MIN,
                        min_label="{V_MIN:0.1f}",
                        max=V_MAX,
                        max_label=f"{V_MAX:0.1f}",
                    ),
                    y_axis=reportcard.Axis(
                        label="Error (¢)",
                        min=-100,
                        min_label="-100",
                        max=100,
                        max_label="+100",
                    ),
                    grid_lines=reportcard.GridLines(
                        x_step=0.1,
                        y_step=0.25,
                    ),
                    center_line=True,
                ),
            ),
        ]
    )

    dest = pathlib.Path(f"calibrations/{gem.serial_number}.afe.json")
    dest.parent.mkdir(parents=True, exist_ok=True)
    with open(dest, "w") as fh:
        json.dump(
            {
                "offset_error": pre_offset_error,
                "gain_error": pre_gain_error,
                "points": calibration_points,
                "pre": pre_measurements,
                "post": post_measurements,
            },
            fh,
            indent=2,
        )
        print(f"[italic]Saved measurement data to {fh.name}[/]")

    return report_section


def _measure_range(gem, hubble, calibration_points, post_measure=lambda x: x):
    output = tui.Updateable()

    results = {}
    diffs = []

    with output:
        for n, (voltage, expected_code) in enumerate(calibration_points.items(), 1):
            print(
                f"[bold]Measuring point {n}/{len(calibration_points)}:[/] {voltage:0.3f} volts"
            )
            print(f"expected:      {expected_code}")

            hubble.VOUT1B.voltage = voltage
            # time.sleep(0.02)

            result = gem.read_adc_average(CHANNEL, SAMPLE_COUNT)
            result = post_measure(result)
            results[voltage] = result

            diff = result - expected_code

            if abs(diff) > 300:
                raise ValueError(
                    f"Reading too far out of range. Expected {expected_code}, measured: {result:.1f}, diff: {diff:.1f}"
                )

            print(f"measured:      {result:.1f}")
            print(f"diff:          {_format_diff(diff)}")

            diffs.append(diff)
            if len(diffs) > 1:
                avg = statistics.mean(diffs)
                dev = statistics.stdev(diffs)
                print(f"avg: {_format_diff(avg)} σ {dev:0.3f}")

            output.update()

    # Exclude the last measurement, since it's *definitely* out of the range
    # and therefore intentionally an outlier
    gain_error, offset_error = adc_errors.calculate_avg_errors(
        list(calibration_points.values()), list(results.values())[:-1]
    )

    return results, gain_error, offset_error
