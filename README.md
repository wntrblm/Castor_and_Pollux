# Winterbloom Castor & Pollux

Castor & Pollux is a Roland Juno-inspired oscillator in Eurorack format.

More details available at https://winterbloom.com/shop/winterbloom-castor-and-pollux

## Repository layout

This repository contains the hardware design files, firmware files, factory setup scripts, and user guide for Castor & Pollux.

- **factory/** contains the scripts used to program, test, and calibrate the module after assembly. These scripts are all written in Python.
- **firmware/** contains the firmware source code, build scripts, and third-party dependencies. The firmware is written in C using GCC ARM.
- **hardware/** contains the hardware design files for the mainboard, jackboard, panel, and programming board. These are all created using KiCAD.
- **user_guide/** contains the user's guide source files. These are written in Markdown and published using MkDocs.

## License and contributing

Castor & Pollux is open-source hardware and software, however, licensing a project like this is a little complicated. Please take a chance to review the [LICENSE](LICENSE.md) file.

We welcome contributions! Please read our [Contributing Guide](CONTRIBUTING.md) and [Code of Conduct](CODE_OF_CONDUCT.md).
