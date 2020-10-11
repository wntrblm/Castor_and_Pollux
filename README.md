# Winterbloom Castor & Pollux

Castor & Pollux is a Roland Juno-inspired oscillator in Eurorack format.

More details available at https://winterbloom.com/winterbloom-castor-pollux

## Repository layout

This repository contains the hardware design files, firmware files, factory setup scripts, and user guide for Castor & Pollux.

- **factory/** contains the scripts used to program, test, and calibrate the module after assembly. These scripts are all written in Python.
- **firmware/** contains the firmware source code, build scripts, and third-party dependencies. The firmware is written in C using GCC ARM.
- **hardware/** contains the hardware design files for the mainboard, jackboard, panel, and programming board. These are all created using KiCAD.
- **user_guide/** contains the user's guide source files. These are written in Markdown and published using MkDocs.

## License and contributing

The code here is available under the [MIT License](firmware/LICENSE), the hardware designs are available under [CC BY-SA 4.0](hardware/LICENSE). I welcome contributors, please read the [Code of Conduct](CODE_OF_CONDUCT.md) first. :)
