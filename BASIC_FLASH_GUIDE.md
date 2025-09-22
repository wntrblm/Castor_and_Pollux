# Basic Flash Guide for Castor & Pollux

This guide is written for someone new to flashing firmware onto hardware, or
needs guidance getting started with Python.

> [!WARNING]
> This guide was not written by a member of the Winterbloom team, but
> was rather contributed by a third-party trying this for the first time. If in
> doubt, [CONTRIUBTING.md](./CONTRIBUTING.md) should be considered
> authoritative.

Assumptions:
  * Access to a computer such that:
    * With Windows installed, and you can use WSL, OR
    * With a flavour of Linux installed, or the ability to install Linux
  * Access to a [black magic debug probe](https://black-magic.org/index.html)
    or similar hardware
  * Access to a bench PSU or similar, and an oscilloscope or tuner to perform
    the calibration.
      * The PSU must be capable of outputting a range of voltages between -0.5V
        and 6.1V accurately
      * The oscilloscope must be capable of indicating the frequency of the
        input.

> [!TIP]
> Consult [the designer's tool list](https://thea.codes/tools.html#testing) for
> the same list of tools used when developing the module. These may be excessive
> for the construction and calibration of a Castor and Pollux module.


## Acquiring the firmware

You will need a copy of the Gemini firmware.

### Current Release

Navigate to the [latest released
firmware](https://github.com/wntrblm/Castor_and_Pollux/releases/latest), and
download the file with the name containing `release` and the suffix `uf2`.

### Building the firmware from head

Starting from a new copy of Arch Linux (CachyOS, WSL, or archlinux.org),
execute the following commands to:

1. Install pre-requisite packages (list may be incomplete)
2. Fetch a copy of this repository
3. Move your current working directory to the repository
4. Move your current working directory to the firmware source root
5. Prepare a python virtual environment
6. Activate the python virtual environment - this functions as an isolated
environment to allow you to install additional dependencies/tooling without
affecting the rest of your system.
7. Install additional dependencies required for setting up the build process
8. Set up the build environment - this will produce a `build.ninja` file, which
you will use in the next step
9. Build the firmware. You should see some graphs showing memory usage and text
indicating the output location.

```
sudo pacman -S arm-none-eabi-{binutils,newlib,gcc,gdb} python3 ninja git
git clone https://github.com/wntrbloom/Castor_and_Pollux
cd Castor_and_Pollux
cd ./firmware
python3 -m venv .
source ./bin/activate
python3 -m pip install -r requirements.txt
python3 ./configure.py
ninja # You should have a C&P firmware.elf and .uf2 at this point
```

## Flashing the firmware

### Connecting to Castor and Pollux

Connect your fimrware flashing hardware of choice to the test points on the
board.

For a Black magic probe, double check the pins using [the debug header
guide](https://black-magic.org/knowledge/pinouts.html#black-magic-debug-unified-connector-bmdu):
  * Pin 1 (vref) - 3v3 test point
  * Pin 3 (gnd) - gnd test point
  * Pin 4 (swdio) - swio test point
  * Pin 6 (swdclk) - swck test point
  * Pin 10 (reset) - rst test point

Finally, I connected Micro-USB input power to the board's USB connector.

### Simple case

If you have a supported probe, flashing the fimrware should be as simple as:

```
cd ./factory
python3 -m venv .
source ./bin/activate
python3 -m pip install -r requirements.txt
python3 ./alt_flash.py
```

### Flashing manually

For me, the `alt_flash.py` script ran into multiple problems, so I ended up
reading the script and running it "manually".

#### Initial firmware

This flashes the board with a utility that allows you to copy-paste the UF2
files to a Castor and Pollux board.

In a separate directory:

```
git clone https://github.com/adafruit/uf2-samdx1
cd uf2-samdx1
make BOARD=winterbloom_gemini
```

Run `arm-none-eabi-gdb` (or equivaluent for your system), and execute the
following commmands. These are based on the [GDB
commands](https://black-magic.org/usage/gdb-commands.html) and [Flashing
automation guide](https://black-magic.org/usage/gdb-automation.html) provided
by Black Magic.

The commands will:

1. Connect to your debug probe.
2. Ensure that the probe monitors the voltage.
3. Scans for any connected debug probe. You should see your device listed here,
   if not, you probably need to check your connection to the board.
4. Attaches to the first (and hopefully only) listed device.
5. "This command instructs GDB to allow access to memory outside of the devices
   known memory map." I can't remember if I ran this, or if it had any effect.
6. Load the relevant executable and transfers it to the device's memory - if the
   `.bin` file doesn't work, try the `.elf`.
7. Verifies that the files match.
8. Execute the program
8. Execute the program
9. Stop executing and restart.
10. Close GDB.

```
target extended-remote /dev/ttyACM0 # Or wherever your probe is mounted
monitor tpwr enable
monitor auto_scan
attach 1
set mem inaccessible-by-default off # 
load ${UF2_DIR}/build/winterbloom_gemini/bootloader-winterbloom_gemini-{...}.bin
compare-sections
start
run
kill
exit
```

Unplug the board, and detach the black magic probe. You should automatically
get a new USB device mounted, to which you can copy-paste the UF2 C&P firmware.
If the device does not appear, it may be necessary to restart your system
before continuing.

## Calibrations

In the same environment as in [#flashing-manually], iterate through the scripts
in the order as listed in [`alt_scripts.md`](./factory/alt_scripts.md),
following the instructions for each calibration step.

At this point, you should have a working and calibrated Castor and Pollux unit.

