import argparse

from wintertools import fw_fetch, jlink, log, noisy_bell

from libgemini import adc_calibration, gemini, ramp_calibration

DEVICE_NAME = "winterbloom_gemini"
JLINK_DEVICE = "ATSAMD21G18"
JLINK_SCRIPT = "scripts/flash.jlink"


def program_firmware():
    log.section("Programming firmware")

    fw_fetch.latest_bootloader(DEVICE_NAME)

    jlink.run(JLINK_DEVICE, JLINK_SCRIPT)


def erase_nvm():
    log.section("Erasing NVM")
    gem = gemini.Gemini.get()
    gem.erase_lut()
    log.success("Erased ramp look-up-table.")
    gem.reset_settings()
    log.success("Erased user settings.")
    gem.soft_reset()
    log.success("Soft reset.")


def run_ramp_calibration():
    log.section("Calibrating ramps")
    ramp_calibration.run(save=True, reset=False)


def run_adc_calibration():
    log.section("Calibrating ADC")

    adc_calibration.run(
        num_calibration_points=20,
        sample_count=128,
        strategy="adc",
        save=True,
    )


def run_afe_calibration():
    log.section("Calibrating AFE")

    adc_calibration.run(
        num_calibration_points=50,
        sample_count=128,
        strategy="afe",
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
        default=["firmware", "ramp", "adc", "afe"],
        help="Select which setup stages to run.",
    )

    args = parser.parse_args()

    if "firmware" in args.stages:
        program_firmware()

    if "erase_nvm" in args.stages:
        erase_nvm()

    if "ramp" in args.stages:
        run_ramp_calibration()

    if "adc" in args.stages:
        run_adc_calibration()

    if "afe" in args.stages:
        run_afe_calibration()

    log.section("Soft-resetting")
    gem = gemini.Gemini.get()
    gem.soft_reset()
    noisy_bell.bell()
    log.success("Finished")


if __name__ == "__main__":
    main()
