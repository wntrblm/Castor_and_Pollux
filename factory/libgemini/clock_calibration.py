# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

import time

from wintertools.print import print
from wintertools import oscilloscope, reportcard

from libgemini import gemini, oscillators

SCOPE_CHANNEL = "c1"
SCOPE_TRIGGER_LEVEL = "0V"
SCOPE_VERTICAL_OFFSET = "0V"
SCOPE_VERTICAL_DIVISION = "1V"
SCOPE_SETTLE_TIME = 2
OSCILLATOR = 1
TEST_NOTE_FREQ = 880
MAX_DEVIATION = 0.3


def run():
    # Calibrates the tuning by indirectly measuring the actual frequency of the
    # SAMD21's internal 8 MHz oscillator.

    scope = oscilloscope.Oscilloscope()
    gem = gemini.Gemini.get()

    print("Configuring scope")
    scope.enable_bandwidth_limit()
    scope.set_intensity(50, 100)

    scope.enable_channel(SCOPE_CHANNEL)
    scope.disable_channel(SCOPE_CHANNEL)
    scope.set_trigger_level(SCOPE_CHANNEL, SCOPE_TRIGGER_LEVEL)
    scope.set_vertical_division(SCOPE_CHANNEL, SCOPE_VERTICAL_DIVISION)
    scope.set_vertical_offset(SCOPE_CHANNEL, SCOPE_VERTICAL_OFFSET)
    scope.set_time_division_from_frequency(TEST_NOTE_FREQ)

    print(f"Changing Gemini note to {TEST_NOTE_FREQ} Hz")
    gem.enter_calibration_mode()
    gem.set_osc8m_freq(8_000_000)
    gem.set_frequency(OSCILLATOR, TEST_NOTE_FREQ)

    print(f"Waiting {SCOPE_SETTLE_TIME}s for scope to settle")
    time.sleep(SCOPE_SETTLE_TIME)

    measured_note_freq = scope.get_freq(SCOPE_CHANNEL)

    print(f"Measured note frequency: {measured_note_freq} Hz")

    period = oscillators.frequency_to_timer_period(880)
    measured_clock_freq = round((period + 1) * measured_note_freq)

    print(f"Measured clock frequency: {measured_clock_freq:,.0f} Hz")

    print("Re-measuring with adjusted clock")

    gem.set_osc8m_freq(measured_clock_freq)
    gem.set_frequency(1, TEST_NOTE_FREQ)

    print(f"Waiting {SCOPE_SETTLE_TIME}s for scope to settle")
    time.sleep(SCOPE_SETTLE_TIME)

    post_measured_note_freq = scope.get_freq(SCOPE_CHANNEL)

    passed = abs(TEST_NOTE_FREQ - post_measured_note_freq) < MAX_DEVIATION

    print(
        f"{'✓' if passed else '!!'} Re-measured note frequency: {post_measured_note_freq:.0f} Hz"
    )

    if passed:
        settings = gem.read_settings()
        settings.osc8m_freq = round(measured_clock_freq)
        gem.save_settings(settings)
        print("✓ Saved to device NVM")

    return reportcard.Section(
        name="Tuning",
        items=[
            reportcard.PassFailItem(label="8 MHz clock", value=passed),
            reportcard.LabelValueItem(
                label="Measured clock frequency",
                value=f"{measured_clock_freq:0,.0f} Hz",
            ),
            reportcard.LabelValueItem(
                label="Measured note frequency", value=f"{measured_note_freq:.3f} Hz"
            ),
            reportcard.LabelValueItem(
                label="Measured note frequency after adjustment",
                value=f"{post_measured_note_freq:0.3f} Hz",
            ),
            reportcard.LabelValueItem(
                label="Tuning error",
                value=f"{abs(TEST_NOTE_FREQ - post_measured_note_freq):0.4f} Hz",
            ),
        ],
    )


if __name__ == "__main__":
    print(run())
