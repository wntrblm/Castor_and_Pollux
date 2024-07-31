import argparse
import pathlib
import time

import rtmidi
import rtmidi.midiutil
from hubble import Hubble
from wintertools import fw_fetch, jlink, reportcard, thermalprinter, fs
from wintertools.print import print

from libgemini import (
    adc_calibration,
    cv_calibration,
    gemini,
    ramp_calibration,
    clock_calibration,
)

DEVICE_NAME = "winterbloom_gemini"
JLINK_DEVICE = "ATSAMD21G18"
JLINK_SCRIPT = "scripts/flash.jlink"
REPORT = reportcard.Report(name="Castor & Pollux")


def erase_nvm():
    print("# Erasing NVM")
    gem = gemini.Gemini.get()
    gem.erase_lut()
    print("✓ Erased ramp look-up-table")
    gem.reset_settings()
    print("✓ Erased user settings")
    gem.soft_reset()
    print("Reset")


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


def has_midi_in_port(port_name):
    midiin = rtmidi.MidiIn(rtmidi.midiutil.get_api_from_environment())
    try:
        ports = midiin.get_ports()
        for port in ports:
            if port_name in port:
                return True
        return False
    finally:
        midiin.delete()


def get_geminiboot():
    try:
        return pathlib.Path(fs.find_drive_by_name("GEMINIBOOT"))
    except RuntimeError:
        return None


def upload_new_bootloader(path):
    print("Copying bootloader...")
    try:
        fs.copyfile(".cache/update-bootloader.winterbloom_gemini.uf2", path / "firmware.uf2")
    except FileNotFoundError:
        pass


def upload_new_firmware(path):
    print("Copying firmware...")
    try:
        fs.copyfile("../firmware/build/gemini-firmware.uf2", path / "firmware.uf2")
    except FileNotFoundError:
        pass


def flash_new_firmware():
    print("Flashing bootloader and firmware...")
    fw_fetch.latest_bootloader(DEVICE_NAME)
    jlink.run(JLINK_DEVICE, JLINK_SCRIPT)


def setup_firmware_and_bootloader():
    geminiboot = get_geminiboot()
    has_midi = has_midi_in_port(gemini.Gemini.MIDI_PORT_NAME)

    # If already connected in the bootloader, copy new bootloader and then
    # copy new firmware.
    if geminiboot:
        print("GEMINIBOOT found, updating bootloader and firmware...")
        upload_new_bootloader(geminiboot)
        fs.wait_for_drive("GEMINIBOOT")
        upload_new_firmware(geminiboot)

    elif has_midi:
        print("Gemini MIDI port found, booting into bootloader and updating bootloader and firmware...")
        gem = gemini.Gemini.get()
        gem.reset_into_bootloader()
        gem.close()
        gemini.Gemini._instance = None

        geminiboot = pathlib.Path(fs.wait_for_drive("GEMINIBOOT"))
        upload_new_bootloader(geminiboot)
        fs.wait_for_drive("GEMINIBOOT")
        upload_new_firmware(geminiboot)
        time.sleep(3)

    else:
        print("No port or drive detected, flashing blank board")
        flash_new_firmware()

    gem = gemini.Gemini.get()
    version = gem.get_firmware_version()

    print("✓ Firmware version {version}")


def main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument(
        "--stages",
        type=str,
        nargs="*",
        default=["firmware", "clock", "ramp", "adc", "cv"],
        help="Select which setup stages to run.",
    )
    parser.add_argument(
        "--force-report",
        type=bool,
        help="Generate a report even if not all steps are run.",
    )

    args = parser.parse_args()

    hubble = Hubble.get()
    hubble.start()

    if "firmware" in args.stages:
        print("# Programming firmware")
        setup_firmware_and_bootloader()

    if "erase_nvm" in args.stages:
        erase_nvm()

    get_firmware_and_serial()

    if "adc" in args.stages:
        print("# Calibrating ADC")
        REPORT.sections.append(adc_calibration.run())

    if "cv" in args.stages:
        print("# Calibrating pitch CV")
        REPORT.sections.append(cv_calibration.run())

    if "clock" in args.stages:
        print("# Calibrating 8 MHz clock")
        REPORT.sections.append(clock_calibration.run())

    if "ramp" in args.stages:
        print("# Calibrating ramps")
        REPORT.sections.append(ramp_calibration.run(save=True))

    gem = gemini.Gemini.get()
    gem.soft_reset()

    if not args.force_report and args.stages != [
        "firmware",
        "clock",
        "ramp",
        "adc",
        "cv",
    ]:
        print(REPORT)
        print("!! Not all stages run, not saving report.")
        return

    print(REPORT)
    REPORT.save()
    reportcard.render_html(REPORT)

    if REPORT.succeeded:
        hubble.success()
        print.success()
    else:
        hubble.fail()
        print.failure()

    thermalprinter.print_me_maybe(reportcard.render_image(REPORT))


if __name__ == "__main__":
    main()
