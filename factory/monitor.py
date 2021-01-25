# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

"""Monitor Gemini's inputs."""

from libgemini import gemini
from libwinter import tui

import time


def color_range_bipolar(v, low, high):
    t = (v - low) / (high - low)
    if t <= 0.5:
        return tui.rgb(tui.gradient((1.0, 0.5, 0.7), (1.0, 1.0, 1.0), t / 0.5))
    else:
        return tui.rgb(tui.gradient((1.0, 1.0, 1.0), (0.6, 1.0, 0.5), (t - 0.5) / 0.5))


def color_range(v, low, high):
    t = (v - low) / (high - low)
    return tui.rgb(tui.gradient((0.5, 0.5, 0.5), (66, 224, 245), t))


def color_range_cv(v, low, high):
    t = (v - low) / (high - low)
    return tui.rgb(tui.gradient((252, 131, 176), (66, 224, 245), t))


def main():
    start = time.monotonic()

    gem = gemini.Gemini()
    gem.enable_monitor()

    column_size = max(tui.width() / 10, 12)
    columns1 = tui.Columns(*[f">{column_size}"] * 6)
    columns2 = tui.Columns(*[f">{column_size}"] * 6)

    output = tui.Updateable()

    with output:
        while True:
            update = gem.monitor()
            columns1.draw(
                output,
                tui.underline,
                "CV A",
                "Knob A",
                "PW A",
                "PW Knob A",
                "LFO",
                "Button",
            )

            columns1.draw(
                output,
                color_range_cv(update.castor_pitch_cv, 0.0, 6.0),
                f"{update.castor_pitch_cv:.3f}v",
                color_range_bipolar(update.castor_pitch_knob, -1.0, 1.0),
                f"{update.castor_pitch_knob:+.3f}v",
                color_range(update.castor_pulse_width_cv, 0, 4095),
                f"{update.castor_pulse_width_cv / 4095 * 100:.0f}%",
                color_range(update.castor_pulse_width_knob, 0, 4095),
                f"{update.castor_pulse_width_knob / 4095 * 100:.0f}%",
                color_range(update.lfo_intensity, 0, 1.0),
                f"{update.lfo_intensity * 100:.0f}%",
                color_range_bipolar(int(update.button_state), 0, 1.0),
                update.button_state,
            )

            columns2.draw(
                output, tui.underline, "CV B", "Knob B", "PW B", "PW Knob B", "", "Time"
            )

            columns2.draw(
                output,
                color_range_cv(update.pollux_pitch_cv, 0.0, 6.0),
                f"{update.pollux_pitch_cv:.3f}v",
                color_range_bipolar(update.pollux_pitch_knob, -1.0, 1.0),
                f"{update.pollux_pitch_knob:+.3f}v",
                color_range(update.pollux_pulse_width_cv, 0, 4095),
                f"{update.pollux_pulse_width_cv / 4095 * 100:.0f}%",
                color_range(update.pollux_pulse_width_knob, 0, 4095),
                f"{update.pollux_pulse_width_knob / 4095 * 100:.0f}%",
                "",
                tui.reset,
                f"{time.monotonic() - start:.0f}",
            )

            output.update()


if __name__ == "__main__":
    main()
