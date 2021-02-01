import readline
import time

from libgemini import fallback_calibration, gemini

readline.parse_and_bind("tab: complete")

gem = gemini.Gemini()
gem.enter_calibration_mode()

settings = None


def _set_oscillators_to_note(note):
    freq = fallback_calibration._midi_note_to_frequency(note)
    period = fallback_calibration._frequency_to_timer_period(freq)
    charge_code = fallback_calibration._estimate_charge_code(freq)
    print(f"Note: {note}, Freq: {freq}, Charge code: {charge_code}")
    gem.set_period(0, period)
    gem.set_dac(0, charge_code, 0)
    time.sleep(0.1)  # Needed so the DAC has time to update EEPROM
    gem.set_period(1, period)
    gem.set_dac(2, charge_code, 0)


midi_note = 11


while True:
    cmd, *vals = input("> ").strip().split()
    vals = [int(val) for val in vals]
    val = vals[0] if vals else None

    if cmd == "note":
        _set_oscillators_to_note(val)

    elif cmd == "next":
        midi_note += 2
        _set_oscillators_to_note(midi_note)

    elif cmd == "sweep":
        for n in range(11, 94):
            _set_oscillators_to_note(n)
            time.sleep(0.5)

    elif cmd == "read_adc":
        print(gem.read_adc(val))

    elif cmd == "set_dac":
        if len(vals) < 2:
            print("requires channel, value, and vref")
        gem.set_dac(vals[0], vals[1], vref=vals[2])

    elif cmd == "set_freq":
        if len(vals) < 2:
            print("requires channel and period")
        gem.set_period(vals[0], vals[1])

    elif cmd == "set_adc_gain":
        gem.set_adc_gain_error_int(val)

    elif cmd == "set_adc_error":
        gem.set_adc_offset_error(val)

    elif cmd == "load_settings":
        settings = gem.read_settings()
        gem_settings = settings
        print(settings)

    elif cmd == "reset_settings":
        gem.reset_settings()
        print(settings)

    elif cmd == "save_settings":
        gem.save_settings(settings)

    elif cmd.startswith("$"):
        eval(cmd[1:])

    else:
        print("unknown command")
