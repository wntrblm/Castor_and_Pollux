# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

"""Monitor Gemini's inputs."""

import pathlib
import time

from wintertools import fs, git, log, tui

from libgemini import gem_monitor_update, gemini


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
    return tui.rgb(tui.gradient((252, 131, 255), (66, 224, 245), t))


SEEN_COLOR = tui.rgb(0, 0, 0) + tui.rgb(100, 255, 100, fg=False)
COLUMNS = tui.Columns("<3", "<10", "<10", ">7", ">10", ">3")
COLUMNS2 = tui.Columns("<3", "^37", ">3")
SPINNER_FRAMES = "🌑🌒🌓🌔🌕🌖🌗🌘"
SPINNER_FRAME_COUNT = len(SPINNER_FRAMES)
spinner_index = 0


def make_seen_states():
    seen_states = gem_monitor_update.GemMonitorUpdate()
    seen_states.castor_pitch_knob = [False, False]
    seen_states.pollux_pitch_knob = [False, False]
    return seen_states


def track_states(update, seen_states):
    if update.castor_pitch_cv > 5.0:
        seen_states.castor_pitch_cv = True
    if update.castor_pitch_knob < 1.0:
        seen_states.castor_pitch_knob[0] = True
    if update.castor_pitch_knob > 1.0:
        seen_states.castor_pitch_knob[1] = True

    if update.pollux_pitch_cv > 5.0:
        seen_states.pollux_pitch_cv = True
    if update.pollux_pitch_knob < 1.0:
        seen_states.pollux_pitch_knob[0] = True
    if update.pollux_pitch_knob > 1.0:
        seen_states.pollux_pitch_knob[1] = True

    if update.castor_pulse_width_cv > 2048:
        seen_states.castor_pulse_width_cv = True
    if update.castor_pulse_width_knob > 2048:
        seen_states.castor_pulse_width_knob = True

    if update.pollux_pulse_width_cv > 2048:
        seen_states.pollux_pulse_width_cv = True
    if update.pollux_pulse_width_knob > 2048:
        seen_states.pollux_pulse_width_knob = True

    if update.button_state is True:
        seen_states.button_state = True

    if update.lfo_intensity > 0.5:
        seen_states.lfo_intensity = True


def draw(update, seen_states, stats=False):
    global spinner_index

    spinner_index = (spinner_index + 1) % SPINNER_FRAME_COUNT
    spinner = SPINNER_FRAMES[spinner_index]

    castor_pitch_cv_color = color_range_cv(update.castor_pitch_cv, 0.0, 6.0)
    castor_pitch_knob_color = color_range_bipolar(update.castor_pitch_knob, -1.0, 1.0)
    pollux_pitch_cv_color = color_range_cv(update.pollux_pitch_cv, 0.0, 6.0)
    pollux_pitch_knob_color = color_range_bipolar(update.pollux_pitch_knob, -1.0, 1.0)

    castor_pw_cv_color = color_range(update.castor_pulse_width_cv, 0, 4095)
    castor_pw_knob_color = color_range(update.castor_pulse_width_knob, 0, 4095)
    pollux_pw_cv_color = color_range(update.pollux_pulse_width_cv, 0, 4095)
    pollux_pw_knob_color = color_range(update.pollux_pulse_width_knob, 0, 4095)

    button_color = color_range_bipolar(int(update.button_state), 0, 1.0)
    lfo_color = color_range(update.lfo_intensity, 0, 1.0)

    castor_pitch_cv_label_color = (
        SEEN_COLOR if seen_states.castor_pitch_cv is True else tui.reset
    )
    castor_pitch_knob_label_color = (
        SEEN_COLOR if seen_states.castor_pitch_knob == [True, True] else tui.reset
    )
    pollux_pitch_cv_label_color = (
        SEEN_COLOR if seen_states.pollux_pitch_cv is True else tui.reset
    )
    pollux_pitch_knob_label_color = (
        SEEN_COLOR if seen_states.pollux_pitch_knob == [True, True] else tui.reset
    )
    castor_pw_cv_label_color = (
        SEEN_COLOR if seen_states.castor_pulse_width_cv is True else tui.reset
    )
    castor_pw_knob_label_color = (
        SEEN_COLOR if seen_states.castor_pulse_width_knob is True else tui.reset
    )
    pollux_pw_cv_label_color = (
        SEEN_COLOR if seen_states.pollux_pulse_width_cv is True else tui.reset
    )
    pollux_pw_knob_label_color = (
        SEEN_COLOR if seen_states.pollux_pulse_width_knob is True else tui.reset
    )
    button_label_color = SEEN_COLOR if seen_states.button_state is True else tui.reset
    lfo_label_color = SEEN_COLOR if seen_states.lfo_intensity is True else tui.reset

    print(f"╔═════════════════════════════════════════╗ {spinner}")
    COLUMNS.draw(
        "║",
        castor_pitch_cv_color,
        f"{update.castor_pitch_cv:+.3f}v",
        castor_pitch_knob_color,
        f"{update.castor_pitch_knob:+.3f}v",
        pollux_pitch_knob_color,
        f"{update.pollux_pitch_knob:+.3f}v",
        pollux_pitch_cv_color,
        f"{update.pollux_pitch_cv:+.3f}v",
        tui.reset,
        "║",
    )
    COLUMNS.draw(
        "║",
        castor_pitch_cv_label_color,
        "CV",
        castor_pitch_knob_label_color,
        "Knob",
        pollux_pitch_knob_label_color,
        "Knob",
        pollux_pitch_cv_label_color,
        "CV",
        tui.reset,
        "║",
    )
    print("║                                         ║")
    COLUMNS.draw(
        "║",
        castor_pw_cv_color,
        f"{update.castor_pulse_width_cv / 4095 * 100:.0f}%",
        castor_pw_knob_color,
        f"{update.castor_pulse_width_knob / 4095 * 100:.0f}%",
        pollux_pw_knob_color,
        f"{update.pollux_pulse_width_knob / 4095 * 100:.0f}%",
        pollux_pw_cv_color,
        f"{update.pollux_pulse_width_cv / 4095 * 100:.0f}%",
        tui.reset,
        "║",
    )
    COLUMNS.draw(
        "║",
        castor_pw_cv_label_color,
        "CV",
        castor_pw_knob_label_color,
        "Knob",
        pollux_pw_knob_label_color,
        "Knob",
        pollux_pw_cv_label_color,
        "CV",
        tui.reset,
        "║",
    )
    print("║                                         ║")

    COLUMNS2.draw(
        "║", button_color, "[X]" if update.button_state else "[ ]", tui.reset, "║"
    )
    COLUMNS2.draw("║", button_label_color, "Btn", tui.reset, "║")
    print("║                                         ║")

    COLUMNS2.draw("║", lfo_color, f"{update.lfo_intensity * 100:.0f}%", tui.reset, "║")
    COLUMNS2.draw("║", lfo_label_color, "LFO", tui.reset, "║")
    print("╚═════════════════════════════════════════╝")

    if stats:
        print(
            f"Loop: {update.loop_time}ms, LEDs: {update.animation_time}ms, ADC: {update.sample_time} cycles"
        )


def _check_firmware_version(gem):
    latest_release = git.latest_tag()
    build_id = gem.get_firmware_version()
    log.info(f"Firmware build ID: {build_id}")

    if latest_release in build_id:
        return

    log.warning(
        "Firmware is out of date, updating it.."
    )

    gem.reset_into_bootloader()

    path = pathlib.Path(fs.wait_for_drive("GEMINIBOOT", timeout=60 * 5))

    fs.copyfile("../firmware/build/gemini-firmware.uf2", path / "firmware.uf2")
    fs.flush(path)

    time.sleep(3)

    log.success("Firmware updated!")


def main(stats=False):
    gem = gemini.Gemini()
    _check_firmware_version(gem)

    settings = gem.read_settings()

    log.info(settings)

    gem.enable_monitor()

    output = tui.Updateable(clear_all=False)

    seen_states = make_seen_states()

    with output:
        while True:
            update = gem.monitor()

            track_states(update, seen_states)
            draw(update, seen_states, stats=stats)

            output.update()


if __name__ == "__main__":
    while True:
        try:
            main()
        except Exception as e:
            if isinstance(e, KeyboardInterrupt):
                raise e
            continue
