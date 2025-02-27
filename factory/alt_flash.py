from wintertools import fw_fetch, jlink, fs
from wintertools.print import print

from libgemini import gemini

import pathlib
import time

import rtmidi
import rtmidi.midiutil

'''

Pre-requisites:
- A J-link compitible USB device
- The J-link USB device connected to the ports on the main board (SWDIO, SWCLK, 3V3, GND, RESET)
- J-Link commander (jlinkexe) installed: https://www.segger.com/products/debug-probes/j-link/tools/j-link-commander/?mtm_campaign=kb&mtm_kwd=J-link-commander
- This machine connected to the main board USB port

'''


DEVICE_NAME = "winterbloom_gemini"
JLINK_DEVICE = "ATSAMD21G18"
JLINK_SCRIPT = "scripts/flash.jlink"

def get_firmware_and_serial():
    print("# Firmware & serial")

    gem = gemini.Gemini.get()
    fw_version = gem.get_firmware_version()
    serial = gem.get_serial_number()

    print(f"Firmware version: {fw_version}")
    print(f"Serial number: {serial}")

def erase_nvm():
    print("# Erasing NVM")
    gem = gemini.Gemini.get()
    gem.erase_lut()
    print("✓ Erased ramp look-up-table")
    gem.reset_settings()
    print("✓ Erased user settings")
    gem.soft_reset()
    print("Reset")

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

def upload_new_firmware(path):
    print("Copying firmware...")
    try:
        fs.copyfile("../firmware/build/gemini-firmware.uf2", path / "firmware.uf2")
    except FileNotFoundError:
        pass

def upload_new_bootloader(path):
    print("Copying bootloader...")
    try:
        fs.copyfile(".cache/update-bootloader.winterbloom_gemini.uf2", path / "firmware.uf2")
    except FileNotFoundError:
        pass

def flash_new_firmware():
    print("Flashing bootloader and firmware...")
    fw_fetch.latest_bootloader(DEVICE_NAME)
    jlink.run(JLINK_DEVICE, JLINK_SCRIPT)

def setup_firmware_and_bootloader(geminiboot=False, has_midi=False):
    geminiboot = get_geminiboot()
    has_midi = has_midi_in_port(gemini.Gemini.MIDI_PORT_NAME)

    print(f"Has Gemini Boot?: {geminiboot}")
    print(f"Has MIDI?: {has_midi}")

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
        print("Resetting into bootloader via SYSEX")
        gem.reset_into_bootloader()
        gem.close()
        gemini.Gemini._instance = None

        print("Please confirm a drive is visible named GEMINIBOOT, if not press the RESET button twice")
        input("Press ENTER to continue")

        geminiboot = pathlib.Path(fs.wait_for_drive("GEMINIBOOT"))
        upload_new_bootloader(geminiboot)
        fs.wait_for_drive("GEMINIBOOT")
        upload_new_firmware(geminiboot)
        time.sleep(3)

    else:
        print("No port or drive detected, flashing blank board")
        flash_new_firmware()

def main():
	
    print()
    print("> This script uploads the bootloader and firmware to your device")
    print("!! Please confirm this machine is connected to the main board USB port and the firmware is present here: ../firmware/build/gemini-firmware.bin")
    print("!! If there is no bootloader on the mainboard, the following must also be true.")
    print("*  A J-link USB device is connected to this machine and to the ports on the main board (SWDIO, SWCLK, 3V3, GND, RESET)")
    print("*  J-Link commander (jlinkexe) installed: https://www.segger.com/products/debug-probes/j-link/tools/j-link-commander/?mtm_campaign=kb&mtm_kwd=J-link-commander")
    print("*  A bootloader is present here: .cache/bootloader.winterbloom_gemini.bin")
    print()
    input("Press ENTER to continue")

    #erase_nvm()
    
    # Flash the boot loader
    print('Flashing Bootloader and Firmware')
    print('........................................')
    setup_firmware_and_bootloader()

if __name__ == "__main__":
    main()
    print()
    input("Flashing complete. Press the RESET button on the main board, then press ENTER to continue")
    print()
    get_firmware_and_serial()