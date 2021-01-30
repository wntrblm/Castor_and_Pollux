import argparse

from wintertools import fw_fetch, jlink, log

from libgemini import adc_calibration, knob_calibration, ramp_calibration

DEVICE_NAME = "winterbloom_gemini"
JLINK_DEVICE = "ATSAMD21G18"
JLINK_SCRIPT = "scripts/flash.jlink"


def program_firmware():
    log.section("Programming firmware")

    fw_fetch.latest_bootloader(DEVICE_NAME)

    jlink.run(JLINK_DEVICE, JLINK_SCRIPT)


def run_adc_calibration():
    log.section("Calibrating ADC")
    # TODO: copy over Sol's code.

    input("Connect Sol output A to Gemini CV A, press enter when ready.")

    adc_calibration.run(
        calibration_points=50,
        sample_count=128,
        adc_range=6.0,
        adc_resolution=2 ** 12,
        adc_channel=0,
        invert=True,
        save=True,
    )


def run_ramp_calibration():
    log.section("Calibrating ramps")
    ramp_calibration.run(save=True)


def run_knob_calibration():
    log.section("Calibrating knob")

    knob_calibration.run(
        adc_resolution=2 ** 12,
        adc_channel=1,
        invert=True,
        save=True,
    )


def main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument(
        "--stages",
        type=str,
        nargs="*",
        default=["firmware", "ramp", "adc", "knob"],
        help="Select which setup stages to run.",
    )

    args = parser.parse_args()

    if "firmware" in args.stages:
        program_firmware()

    if "ramp" in args.stages:
        run_ramp_calibration()

    if "adc" in args.stages:
        run_adc_calibration()

    if "knob" in args.stages:
        run_knob_calibration()


if __name__ == "__main__":
    main()
