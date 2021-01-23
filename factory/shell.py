import readline
from libgemini import gemini

readline.parse_and_bind("tab: complete")

gem = gemini.Gemini()
gem.enter_calibration_mode()

settings = None

while True:
    cmd, *vals = input("> ").strip().split()
    vals = [int(val) for val in vals]
    val = vals[0] if vals else None

    if cmd == "read_adc":
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
