#!/usr/bin/env python3

import math
import sys
import subprocess

if(len(sys.argv) != 4):
    print("Usage: get-fw-size.py ELF_FILE FLASH_SIZE RAM_SIZE")
    sys.exit(-1)

_, infile, max_flash, max_ram = sys.argv
flash_size = int(max_flash, 0)
ram_size = int(max_ram, 0)

fw_size_output = subprocess.check_output(["arm-none-eabi-size", infile])
fw_size_output = fw_size_output.decode("utf-8").split("\n")[1]
text, data, bss, *_ = [x.strip() for x in fw_size_output.split(" ") if x.strip()]
text = int(text, 10)
data = int(data, 10)
bss = int(bss, 10)

flash_used = text + data
flash_used_percent = math.ceil(flash_used / flash_size * 100)
ram_used = data + bss
ram_used_percent = math.ceil(ram_used / ram_size * 100)

colors = [57, 92, 127, 162, 197]
reset = "\u001b[0m"

print(f"Flash used: {flash_used} / {flash_size} ({flash_used_percent}%)")
color = "\u001b[38;5;{}m".format(colors[math.floor(flash_used_percent / 100 * (len(colors) -1))])
print(color,"█" * math.floor(flash_used_percent / 4), "▒" * math.ceil((100 - flash_used_percent) / 4), reset, sep="")

print(f"Flash used: {ram_used} / {ram_size} ({ram_used_percent}%)")
color = "\u001b[38;5;{}m".format(colors[math.floor(ram_used_percent / 100 * (len(colors) - 1))])
print(color, "█" * math.floor(ram_used_percent / 4), "▒" * math.ceil((100 - ram_used_percent) / 4), reset, sep="")