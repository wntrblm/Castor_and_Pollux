# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

"""Update firmware on programmed C&P boards"""

import pathlib
import sys
import time

from wintertools import fs, git, tui

from libgemini import gemini

def _check_firmware_version(gem):
    latest_release = git.latest_tag()
    build_id = gem.get_firmware_version()
    print(f"Firmware build ID: {build_id}")

    if latest_release in build_id:
        return True
    else:
        return False


def _update_firmware(gem):
    print("Updating firmware..")

    gem.reset_into_bootloader()

    path = pathlib.Path(fs.wait_for_drive("GEMINIBOOT", timeout=60 * 5))

    fs.copyfile("../firmware/build/gemini-firmware.uf2", path / "firmware.uf2")
    fs.flush(path)

    time.sleep(3)

    print("[green]Firmware updated![/]")

    build_id = gem.get_firmware_version()
    print(f"Firmware build ID: {build_id}")


def main(stats=False):
    gem = gemini.Gemini.get()
    _update_firmware(gem)


if __name__ == "__main__":
    main()
