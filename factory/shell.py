from libgemini import gemini

gem = gemini.Gemini()
gem.enter_calibration_mode()

while True:
    cmd, vals = input().strip().split(1)
    vals = [int(val) for val in vals.split()]
    val = vals[0]

    if cmd == "adc":
        print(gem.read_adc(val))

    if cmd == "dac":
        if len(vals) < 2:
            print("requires channel, value, and gain")
        gem.set_dac(vals[0], vals[1], gain=vals[2])

    if cmd == "freq":
        if len(vals) < 2:
            print("requires channel and period")
        gem.set_period(vals[0], vals[1])

    if cmd == "adcgain":
        gem.set_adc_gain_error(val)

    if cmd == "adcerror":
        gem.set_adc_offset_error(val)