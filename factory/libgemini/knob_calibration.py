# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

import argparse
import statistics
import pathlib
import json

from libgemini import gemini


def run(adc_resolution, invert, adc_channel, save):
    if invert:
        high_expected = 0
        low_expected = adc_resolution - 1
    else:
        high_expected = adc_resolution - 1
        low_expected = 0

    gem = gemini.Gemini()

    gem.enter_calibration_mode()

    # Knob calibration is done with error correction enabled.
    gem.enable_adc_error_correction()

    input(f"Set knob for channel {adc_channel} all the way CCW and press enter.")

    samples = []
    for s in range(128):
        samples.append(gem.read_adc(adc_channel))

    low_measured = statistics.mean(samples)

    print(f"> Measured {low_measured}, expected {low_expected}")

    input(f"Set knob for channel {adc_channel} all the way CW and press enter.")

    samples = []
    for s in range(128):
        samples.append(gem.read_adc(adc_channel))

    high_measured = statistics.mean(samples)

    print(f"> Measured {high_measured}, expected {high_expected}")

    gain_error = (high_expected - low_expected) / (high_measured - low_measured)
    offset_error = (low_measured * gain_error) - low_expected

    print(f"Knob gain error: {gain_error:.3f}, offset error: {offset_error:.1f}")

    local_copy = pathlib.Path("calibrations") / f"{gem.serial_number}.knob.json"
    local_copy.parent.mkdir(parents=True, exist_ok=True)

    with local_copy.open("w") as fh:
        json.dump({"gain_error": gain_error, "offset_error": offset_error}, fh)

    print(f"Saved local copy to {local_copy}")

    if save:
        gain_f16 = int(gain_error * 0x10000)
        offset_f16 = int(offset_error * 0x10000)

        settings = gem.read_settings()
        settings.knob_gain_corr = gain_f16
        settings.knob_offset_corr = offset_f16

        gem.save_settings(settings)
        print("Saved to NVM.")
    else:
        print("Dry run, not saved to NVM.")

    print("Done")
    gem.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument(
        "--adc_resolution", type=int, default=2 ** 12, help="ADC resolution."
    )
    parser.add_argument(
        "--adc_channel",
        type=int,
        default=1,
        help="Which of Gemini's ADC channels to measure.",
    )
    parser.add_argument(
        "--invert",
        action="store_true",
        default=False,
        help="Indicates that the bottom of the voltage range represents the top of the ADC code range",
    )
    parser.add_argument(
        "--dry_run",
        action="store_true",
        default=False,
        help="Don't save the calibration values.",
    )

    args = parser.parse_args()

    run(args.adc_resolution, args.invert, args.adc_channel, not args.dry_run)
