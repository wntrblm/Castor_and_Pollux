# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

import json
import pathlib
import time

from wintertools.print import print
from wintertools import oscilloscope, reportcard

from libgemini import gemini, oscillators

SCOPE_SETTLE_TIME = 4
OSCILLATOR = 0
TEST_NOTE_FREQ = 880
MAX_DEVIATION = 0.85

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

def run():
    # Calibrates the tuning by indirectly measuring the actual frequency of the
    # SAMD21's internal 8 MHz oscillator.

    input("\nConnect a scope to Castor Output (Left hand side). Then turn up the SAW output on Castor (left hand side) and press enter....")
    gem = gemini.Gemini.get()
    print(f"Changing Gemini note to {TEST_NOTE_FREQ} Hz")
    gem.enter_calibration_mode()
    gem.set_osc8m_freq(8_000_000)
    gem.set_frequency(OSCILLATOR, TEST_NOTE_FREQ)
    gem.set_dac(2048, 2048, 2048, 2048)

    print(f"Waiting {SCOPE_SETTLE_TIME}s for scope to settle")
    time.sleep(SCOPE_SETTLE_TIME)

    # measured_note_freq = scope.get_freq(SCOPE_CHANNEL)
    measured_note_freq = float(input("\nEnter Measured Frequency from scope...."))

    print(f"Measured note frequency: {measured_note_freq} Hz")

    period = oscillators.frequency_to_timer_period(880)
    measured_clock_freq = round((period + 1) * measured_note_freq)

    print(f"Measured clock frequency: {measured_clock_freq:,.0f} Hz")

    print("Re-measuring with adjusted clock")

    gem.set_osc8m_freq(measured_clock_freq)
    gem.set_frequency(OSCILLATOR, TEST_NOTE_FREQ)

    print(f"Waiting {SCOPE_SETTLE_TIME}s for scope to settle")
    time.sleep(SCOPE_SETTLE_TIME)

    # post_measured_note_freq = scope.get_freq(SCOPE_CHANNEL)
    post_measured_note_freq = float(input("\nEnter Measured Frequency from scope...."))

    passed = abs(TEST_NOTE_FREQ - post_measured_note_freq) < MAX_DEVIATION

    print()
    if passed:
        print.success()
    else:
        print.failure()
        print(f"Calibration failed: post_zero_code < 20 or post_max_code < 4075")
    print()

    print(
        f"{'✓' if passed else '!!'} Re-measured note frequency: {post_measured_note_freq:.0f} Hz"
    )

    if passed:
        settings = gem.read_settings()
        settings.osc8m_freq = round(measured_clock_freq)
        gem.save_settings(settings)
        print()
        print("✓ Saved to device NVM")

    local_copy = pathlib.Path("calibrations") / f"{gem.get_serial_number()}.clock.json"
    local_copy.parent.mkdir(parents=True, exist_ok=True)

    with local_copy.open("w") as fh:
        data = {
            "expected_note_frequency": TEST_NOTE_FREQ,
            "measured_note_frequency": measured_note_freq,
            "measured_clock_frequency": measured_clock_freq,
        }
        json.dump(data, fh)
        print(f"[italic]Saved to {local_copy}[/]")

    return reportcard.Section(
        name="Tuning",
        items=[
            reportcard.PassFailItem(label="8 MHz clock", value=passed),
            reportcard.LabelValueItem(
                label="Measured clock",
                value=f"{measured_clock_freq:0,.0f} Hz",
            ),
            reportcard.LabelValueItem(
                label="Measured note", value=f"{measured_note_freq:.3f} Hz"
            ),
            reportcard.LabelValueItem(
                label="Adjusted note",
                value=f"{post_measured_note_freq:0.3f} Hz",
            ),
            reportcard.LabelValueItem(
                label="Tuning error",
                value=f"{abs(TEST_NOTE_FREQ - post_measured_note_freq):0.4f} Hz",
            ),
        ],
    )


if __name__ == "__main__":
    
    print()
    print("> This script calibrates the 8kHz clock by connecting a scope to the Castor output (Left hand side)")
    print("!! Please confirm the following are true, then press ENTER to continue:")
    print("* This machine connected to the main board USB port")
    print("* There is not a drive visible named GEMINIBOOT. If so, please power cycle the main board")
    input()

    REPORT = reportcard.Report(name="Castor & Pollux")
    get_firmware_and_serial()
    print()
    
    REPORT.sections.append(run())
    print(REPORT)
