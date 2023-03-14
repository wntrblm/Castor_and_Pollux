# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

from wintertools import reportcard
from wintertools.print import print

from libgemini import adc_errors, gemini

# The Hubble board wires the chorus pot to board ground.
ZERO_CODE_CHANNEL = gemini.Gemini.ADC.CHORUS_POT
# The Hubble board wires the pitch pots to board 3.3v.
MAX_CODE_CHANNEL = gemini.Gemini.ADC.CV_A_POT


def run():
    # Simple ADC calibration since this is really only used to read the
    # potentiometers. The pitch CV input has an additionally, more complex
    # calibration that's measured and applied.

    gem = gemini.Gemini.get()

    gem.enter_calibration_mode()
    gem.disable_adc_error_correction()

    zero_code = gem.read_adc_average(ZERO_CODE_CHANNEL)
    max_code = gem.read_adc_average(MAX_CODE_CHANNEL)

    gain_error, offset_error = adc_errors.calculate(0, 4095, zero_code, max_code)

    print("Pre-calibration measurements:")
    print(f"* 0v : {zero_code}")
    print(f"* 3.3v : {max_code}")
    print(f"* Gain error : {gain_error:0.3f}")
    print(f"* Offset error : {offset_error:.0f}")

    gem.set_adc_gain_error(gain_error)
    gem.set_adc_offset_error(offset_error)
    gem.enable_adc_error_correction()
    print("✓ Saved to device NVM")

    post_zero_code = gem.read_adc_average(ZERO_CODE_CHANNEL)
    post_max_code = gem.read_adc_average(MAX_CODE_CHANNEL)
    post_gain_error, post_offset_error = adc_errors.calculate(
        0, 4095, post_zero_code, post_max_code
    )

    print("Post-calibration measurements:")
    print(f"* 0v : {post_zero_code}")
    print(f"* 3.3v : {post_max_code}")
    print(f"* Gain error : {post_gain_error:0.3f}")
    print(f"* Offset error : {post_offset_error:.0f}")

    passed = post_zero_code < 20 and post_max_code > 4075

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
