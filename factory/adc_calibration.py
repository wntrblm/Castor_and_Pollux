import argparse
import time
import statistics

from libgemini import gemini
from libgemini import sol
from libgemini import adc_errors


def main(calibration_points, adc_range, adc_resolution, invert, adc_channel, save):
    voltages = [
        n / calibration_points * adc_range
        for n in range(calibration_points + 1)
    ]

    expected_codes = [
        int(voltages[n] / adc_range * (adc_resolution - 1))
        for n in range(calibration_points + 1)
    ]

    if invert:
        expected_codes = [adc_resolution - 1 - code for code in expected_codes]

    measured_codes = []

    gem = gemini.Gemini()
    sol_ = sol.Sol()

    gem.enter_calibration_mode()

    # TODO: Change to "send_voltage"
    sol_.send_voltage(0)

    for n in range(calibration_points + 1):
        voltage = n / calibration_points * adc_range
        print(f"Measuring {voltage}, expecting {expected_codes[n]}.")
        sol_.send_voltage(voltage)
        time.sleep(0.2)
        result = gem.read_adc(adc_channel)
        print(f"Got {result}, diff {result - expected_codes[n]}")
        measured_codes.append(result)

    gain_error = adc_errors.calculate_avg_gain_error(expected_codes, measured_codes)
    offset_error = adc_errors.calculate_avg_offset_error(expected_codes, measured_codes, gain_error)
    print(f"Measured: Gain: {gain_error}, Offset: {offset_error}")

    corrected = adc_errors.apply_correction(measured_codes, gain_error, offset_error)
    corrected_gain_error = adc_errors.calculate_avg_gain_error(expected_codes, corrected)
    corrected_offset_error = adc_errors.calculate_avg_offset_error(expected_codes, corrected, corrected_gain_error)
    print(f"Corrected: Gain: {corrected_gain_error}, Offset: {corrected_offset_error}")

    if save:
        gem.set_adc_gain_error(gain_error)
        gem.set_adc_offset_error(int(offset_error))
        print("Saved to NVM.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("--calibration_points", type=int, default=100, help="Number of calibration points.")
    parser.add_argument("--adc_range", type=float, default=3.3, help="ADC range (in volts).")
    parser.add_argument("--adc_resolution", type=int, default=2**12, help="ADC resolution.")
    parser.add_argument("--adc_channel", type=int, default=0, help="Which of Gemini's ADC channels to measure.")
    parser.add_argument("--invert", action="store_true", default=False, help="Indicates that the bottom of the voltage range represents the top of the ADC code range")
    parser.add_argument("--dry_run", action="store_true", default=False, help="Don't save the calibration values.")

    args = parser.parse_args()

    main(args.calibration_points, args.adc_range, args.adc_resolution, args.invert, args.adc_channel, not args.dry_run)