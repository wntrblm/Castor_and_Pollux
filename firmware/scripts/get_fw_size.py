#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Stargirl Flowers (@theacodes) 2020-2021

import argparse
import colorsys
import collections
import dataclasses
import json
import math
import pathlib
import sys
import subprocess
import typing

COLUMNS = ["<15", ">8", "^5", ">8", ">7"]
COLUMNS_ALT = ["<15", ">8", "<13", ">7"]
FIXED_SEG_COLOR = (255, 158, 221)
BAR_FILL_CHAR = "░"
BAR_FILL_COLOR = (0.4, 0.4, 0.4)
GRADIENT_START = colorsys.hsv_to_rgb(188 / 360, 0.8, 1.0)
GRADIENT_END = colorsys.hsv_to_rgb(0.8, 0.8, 1.0)
PLUS_COLOR = (1.0, 1.0, 0.5)
MINUS_COLOR = (127, 255, 191)


class TermColor:
    reset = "\u001b[0m"

    @staticmethod
    def gradient(a, b, v):
        r = a[0] + v * (b[0] - a[0])
        g = a[1] + v * (b[1] - a[1])
        b = a[2] + v * (b[2] - a[2])
        return r, g, b

    @staticmethod
    def esc(r, g=None, b=None):
        if isinstance(r, tuple):
            r, g, b = r

        if r > 1 or g > 1 or b > 1:
            r, g, b = r / 255, g / 255, b / 255

        r, g, b = [int(x * 255) for x in (r, g, b)]
        return f"\u001b[38;2;{r};{g};{b}m"


class TermUI:
    @staticmethod
    def segmented_bar(length, *segments, fill=False):
        segments = list(segments)

        # Add end segment if needed.
        if fill:
            left_to_fill = 1.0 - sum(s["l"] for s in segments)
            segments.append(dict(l=left_to_fill, c=BAR_FILL_COLOR, p=BAR_FILL_CHAR))

        # Largest remainder method allocation
        seg_lengths = [math.floor(s["l"] * length) for s in segments]
        seg_fract = [(n, (s["l"] * length) % 1.0) for n, s in enumerate(segments)]
        seg_fract.sort(key=lambda x: x[1], reverse=True)
        remainder = length - sum(seg_lengths)

        for n in range(remainder):
            seg_lengths[seg_fract[n][0]] += 1

        # Now draw
        for n, seg in enumerate(segments):
            print(
                TermColor.esc(*seg["c"]),
                seg.get("p", "▓") * seg_lengths[n],
                sep="",
                end="",
            )

        print(end="\n")

    @staticmethod
    def columnize(columns, *values):
        n = 0
        for v in values:
            if isinstance(v, str) and v.startswith("\u001b"):
                print(v, end="")
                continue
            if isinstance(v, tuple) and len(v) == 3:
                print(TermColor.esc(v), end="")
                continue

            c = columns[n]
            formatter = f"{{: {c}}}"
            print(formatter.format(v), end="")

            n += 1

        print(TermColor.reset, end="\n")

    @staticmethod
    def columns_length(columns):
        return sum(int(s[1:]) for s in columns)


BAR_LEN = TermUI.columns_length(COLUMNS)


def analyze_elf(elf, size_prog):
    fw_size_output = subprocess.check_output([size_prog, "-A", "-d", elf])
    fw_size_output = fw_size_output.decode("utf-8").split("\n")[2:]
    sections = {}
    bootloader_size = 0

    for line in fw_size_output:
        if not line:
            continue
        parts = line.split(None)
        sections[parts[0]] = int(parts[1], 10)
        if parts[0] == ".text":
            bootloader_size = int(parts[2], 10)

    program_size = sections[".text"] + sections.get(".relocate", 0) + sections.get(".data", 0)
    stack_size = sections[".stack"]
    variables_size = sections.get(".relocate", 0) + sections.get(".data", 0) + sections[".bss"]

    return bootloader_size, program_size, stack_size, variables_size


def color_for_percent(percentage):
    return TermColor.gradient(GRADIENT_START, GRADIENT_END, percentage)


@dataclasses.dataclass
class MemorySection:
    name: str
    size: int
    last_size: typing.Optional[int] = None
    fixed: bool = False


def print_memory_sections(name, size, *sections):
    used = sum(s.size for s in sections)
    used_fixed = sum(s.size for s in sections if s.fixed)
    used_percent = used / size
    color = color_for_percent(used_percent)

    TermUI.columnize(
        COLUMNS,
        f"{name} used:",
        color,
        f"{used:,}",
        TermColor.reset,
        "/",
        f"{size:,}",
        color,
        f"({round(used_percent * 100)}%)",
    )

    TermUI.segmented_bar(
        BAR_LEN,
        dict(l=used_fixed / size, c=FIXED_SEG_COLOR),
        dict(l=(used - used_fixed) / size, c=color_for_percent(used_percent)),
        fill=True,
    )

    for sec in sections:
        if sec.fixed:
            color = FIXED_SEG_COLOR
        else:
            color = color_for_percent(sec.size / size)

        if sec.last_size is not None:
            diff = sec.size - sec.last_size
            if diff != 0:
                last_size_sec = (
                    MINUS_COLOR if diff < 0 else PLUS_COLOR,
                    f" {diff:+,}",
                    TermColor.reset,
                )
            else:
                last_size_sec = ("",)
        else:
            last_size_sec = ("",)

        TermUI.columnize(
            COLUMNS_ALT,
            color,
            f"{sec.name}: ",
            f"{sec.size:,}",
            *last_size_sec,
            color,
            f"({round(sec.size / size * 100)}%)",
        )


def main():
    parser = argparse.ArgumentParser("get_fw_size.py")
    parser.add_argument("elf_file", type=pathlib.Path)
    parser.add_argument("--flash-size", type=lambda x: int(x, 0))
    parser.add_argument("--ram-size", type=lambda x: int(x, 0))
    parser.add_argument("--no-last", type=bool, default=False)
    parser.add_argument("--size-prog", type=pathlib.Path, default="arm-none-eabi-size")

    args = parser.parse_args()

    build_dir = args.elf_file.parent
    last_file = build_dir / "fw-size.last"

    if last_file.exists():
        last_data = json.loads(last_file.read_text())
        last_program_size = last_data["program_size"]
        last_variables_size = last_data["variables_size"]
    else:
        last_program_size = None
        last_variables_size = None

    bootloader_size, program_size, stack_size, variables_size = analyze_elf(
        args.elf_file, args.size_prog
    )
    if last_file.exists():
        last_data = json.loads(last_file.read_text())

    print_memory_sections(
        "Flash",
        args.flash_size,
        MemorySection(name="Bootloader", size=bootloader_size, fixed=True),
        MemorySection(name="Program", size=program_size, last_size=last_program_size),
    )
    print()
    print_memory_sections(
        "RAM",
        args.ram_size,
        MemorySection(name="Stack", size=stack_size, fixed=True),
        MemorySection(
            name="Variables", size=variables_size, last_size=last_variables_size
        ),
    )

    if not args.no_last:
        last_file.write_text(
            json.dumps(
                dict(
                    program_size=program_size,
                    variables_size=variables_size,
                )
            )
        )


if __name__ == "__main__":
    main()
