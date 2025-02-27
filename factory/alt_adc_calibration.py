from wintertools import reportcard
from wintertools.print import print
from libgemini import adc_errors, gemini


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

    # The Hubble board wires the chorus pot to board ground. We must do it manually
    input("\nSet LFO/CHORUS POT to full CCW, then press Enter to continue...")
    ZERO_CODE_CHANNEL = gemini.Gemini.ADC.CHORUS_POT
    # The Hubble board wires the pitch pots to board 3.3v. We must do it manually
    input("\nSet PITCH POT A (Left hand side) to full CW, then press Enter to continue...")
    MAX_CODE_CHANNEL = gemini.Gemini.ADC.CV_A_POT

    # Simple ADC calibration since this is really only used to read the
    # potentiometers. The pitch CV input has an additionally, more complex
    # calibration that's measured and applied.

    gem = gemini.Gemini.get()
    print("Entering Calibration Mode and disabling ADC error correction...\n")
    gem.enter_calibration_mode()
    gem.disable_adc_error_correction()

    zero_code = gem.read_adc_average(ZERO_CODE_CHANNEL)
    max_code = gem.read_adc_average(MAX_CODE_CHANNEL)


    gain_error, offset_error = adc_errors.calculate(0, 4095, zero_code, max_code)
    
    print()
    print("Pre-calibration measurements:")
    print(f"* 0v : {zero_code}")
    print(f"* 3.3v : {max_code}")
    print(f"* Gain error : {gain_error:0.3f}")
    print(f"* Offset error : {offset_error:.0f}")

    gem.set_adc_gain_error(gain_error)
    gem.set_adc_offset_error(offset_error)
    gem.enable_adc_error_correction()
    print("\n✓ Saved to device NVM\n")

    post_zero_code = gem.read_adc_average(ZERO_CODE_CHANNEL)
    post_max_code = gem.read_adc_average(MAX_CODE_CHANNEL)
    post_gain_error, post_offset_error = adc_errors.calculate(
        0, 4095, post_zero_code, post_max_code
    )

    print()
    print("Post-calibration measurements:")
    print(f"* 0v : {post_zero_code}")
    print(f"* 3.3v : {post_max_code}")
    print(f"* Gain error : {post_gain_error:0.3f}")
    print(f"* Offset error : {post_offset_error:.0f}")

    passed = post_zero_code < 20 and post_max_code > 4075

    print()
    if passed:
    	print.success()
    else:
    	print.failure()

    return reportcard.Section(
        name="ADC",
        items=[
            reportcard.PassFailItem(label="Calibration", value=passed),
            reportcard.LabelValueItem(label="Gain error", value=f"{gain_error:0.3f}"),
            reportcard.LabelValueItem(
                label="Offset error", value=f"{offset_error:.0f}"
            ),
            reportcard.LabelValueItem(
                label="Adj gain error", value=f"{post_gain_error:0.3f}"
            ),
            reportcard.LabelValueItem(
                label="Adj offset error", value=f"{post_offset_error:.0f}"
            ),
        ],
    )


if __name__ == "__main__":
    
    print()
    print("> This script calibrates the ADC using values from the LFO/Chorus Pot and Pitch CV Pot A (Left hand side)")
    print("!! Please confirm the following are true, then press ENTER to continue:")
    print("* This machine connected to the main board USB port")
    print("* There is not a drive visible named GEMINIBOOT. If so, please power cycle the main board")
    print("* The main board is connected to eurorack power")
    input()

    REPORT = reportcard.Report(name="Castor & Pollux")
    get_firmware_and_serial()

    gem = gemini.Gemini.get()
    print()
    print("Pre-calibration settings...\n")
    print(gem.read_settings())

    REPORT.sections.append(run())

    print()
    print("Post-calibration settings...\n")
    print(gem.read_settings())

    print(REPORT)
