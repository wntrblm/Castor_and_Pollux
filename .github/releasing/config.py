# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

import os
import os.path
import subprocess

import jinja2
from __main__ import add_artifact
from wintertools import build_info

message_template = jinja2.Template(
    open(os.path.join(os.path.dirname(__file__), "template.jinja2"), "r").read()
)


def prepare_artifacts(info):
    firmware_dir = os.path.join("firmware")

    print("Building debug artifacts...")

    os.chdir(firmware_dir)
    subprocess.run(["python3", "configure.py", "--config", "debug"])
    subprocess.run(["ninja"])
    build_id = build_info.extract_compiled_build_info(
        "build/build/generated_build_info.o"
    )
    info["debug_build_id"] = build_id

    add_artifact(
        "build/gemini-firmware.elf",
        f"gemini-firmware-{info['tag']}-debug.elf",
    )
    add_artifact(
        "build/gemini-firmware.uf2",
        f"gemini-firmware-{info['tag']}-debug.uf2",
    )

    print("Building release artifacts...")

    subprocess.run(["python3", "configure.py", "--config", "release"])
    subprocess.run(["ninja"])
    build_id = build_info.extract_compiled_build_info(
        "build/build/generated_build_info.o"
    )
    info["release_build_id"] = build_id

    add_artifact(
        "build/gemini-firmware.elf",
        f"gemini-firmware-{info['tag']}-release.elf",
    )
    add_artifact(
        "build/gemini-firmware.uf2",
        f"gemini-firmware-{info['tag']}-release.uf2",
    )


def prepare_description(info, artifacts):
    return message_template.render(artifacts=artifacts, **info)
