#!/usr/bin/env python3

import argparse
import pathlib
import shutil
import subprocess
import sys

# Application-specific sources, includes, and defines.

PROGRAM = "gemini-firmware"

SRCS = [
    "src/**/*.c",
    # The generated build info source file. It provides various details about
    # the build to the program. The header is at src/lib/gem_build_info.h
    "build/generated_build_info.c",
    # libwinter is common winterbloom code.
    "third_party/libwinter/*.c",
    # Contains the main entrypoint for the processor and the ARM exception
    # table. TODO: this should be pulled into the application.
    "third_party/samd21/gcc/gcc/startup_samd21.c",
    # Tiny printf
    "third_party/printf/*.c",
    # For fix16_t
    "third_party/libfixmath/*.c",
    # Structy
    "third_party/structy/*.c",
    # SEGGER's RTT for transferring printf output back to the host.
    "third_party/rtt/SEGGER_RTT.c",
    # We're a bit more picky about TinyUSB's sources, since we only use a very
    # small bit of TinyUSB's functionality.
    "third_party/tinyusb/src/tusb.c",
    "third_party/tinyusb/src/class/midi/midi_device.c",
    "third_party/tinyusb/src/common/tusb_fifo.c",
    "third_party/tinyusb/src/device/usbd.c",
    "third_party/tinyusb/src/device/usbd_control.c",
    "third_party/tinyusb/src/portable/microchip/samd/dcd_samd.c",
]

INCLUDES = [
    "third_party/samd21/include",
    "third_party/cmsis/include",
]

DEFINES = dict(
    # Defines for TinyUSB
    CFG_TUSB_MCU="OPT_MCU_SAMD21",
    # Defines for wntr:
    # - Enable the ARM Microtrace Buffer which can help with debugging.
    WNTR_ENABLE_MTB=1,
    # Defines for libfixmath:
    # - Use a faster, less accurate sin() function.
    FIXMATH_FAST_SIN=1,
    # - Don't cache look-up tables, Gemini can't fit them all in RAM.
    FIXMATH_NO_CACHE=1,
    # Defines for printf:
    # - Disable float support since we don't use floats in Gemini.
    PRINTF_DISABLE_SUPPORT_FLOAT=1,
    PRINTF_DISABLE_SUPPORT_EXPONENTIAL=1,
)


# MCU configuration


# The MCU has a few variants, called "devices" in the CMSIS headers. Each one
# has its own header file and linker script. Gemini uses the SAMD21G18A.
MCU = "SAMD21G18A"

# CPU type for GCC, for example:
# - cortex-m0plus
# - cortex-m4
# - cortex-m7
CPU = "cortex-m0plus"

# Floating point unit type.
# - Cortex M0+: auto (emulated)
# - Cortex M4F: fpv4-sp-d16
# - Cortex M7: fpv5-sp-d16
FPU = "auto"

# Floating-point binary interface. Must be soft, softfp, or hard.
# - For Cortex M0+ this should be soft.
# - For Cortex M4F and M7F this should be hard.
FLOAT_ABI = "soft"

# Size of the MCU variant's memories. These are all specified in the datasheet.
FLASH_SIZE = 0x40000
RAM_SIZE = 0x8000

# The amount of SRAM to set aside for the program stack.
STACK_SIZE = 0x800

# Each device has a seperate linker script. This was copied from
# third_party/samd21/gcc
LINKER_SCRIPT = f"scripts/{MCU.lower()}.ld"


# Translate the options above into necessary defines.

# Used in CMSIS device support header. See sam.h.
DEFINES[f"__{MCU}__"] = 1

# Used in CMSIS math headers.
# see https://github.com/ARM-software/CMSIS/blob/master/CMSIS/Include/arm_math.h#L84-L88
if CPU == "cortex-m0plus":
    DEFINES["ARM_MATH_CM0PLUS"] = 1
elif CPU == "cortex-m4":
    DEFINES["ARM_MATH_CM4"] = 1
elif CPU == "cortex-m7":
    DEFINES["ARM_MATH_CM7"] = 1


# Toolchain configuration

GCC = "arm-none-eabi-gcc"
OBJCOPY = "arm-none-eabi-objcopy"

COMMON_FLAGS = [
    # Tell GCC which processor we're using and whether it has floating-point
    # Hardware.
    f"-mcpu={CPU}",
    f"-mfloat-abi={FLOAT_ABI}",
    f"-mfpu={FPU}",
    # Select C11 + GNU extensions as the program's C dialect.
    "--std=gnu11",
    # Use newlib-nano, a very minimal libc.
    "--specs=nano.specs",
    # Cortex-M CPUs only support the Thumb instruction set.
    "-mthumb",
    # Tells the compiler to use ARM's EABI with variable-length enums.
    # The alternative is aapcs-linux which is the same with fixed-length
    # 4-byte enums.
    # AAPCS here refers to the ARM Architecture Procedure Call Standard.
    # https://developer.arm.com/documentation/ihi0042/j/?lang=en
    "-mabi=aapcs",
    # Set some C dialect options that are useful for embedded development:
    # - char is always unsigned.
    # - bitfields are always unsigned.
    # - put each enum into the smallest type that'll hold its values.
    "-funsigned-char -funsigned-bitfields -fshort-enums",
    # Enable unused code/data elimination. This flag makes the compiler generate
    # individual sections for each static variable and function instead of
    # combining them together. The linker can cull them during linking.
    # https://interrupt.memfault.com/blog/best-and-worst-gcc-clang-compiler-flags#-ffunction-sections--fdata-sections----gc-sections
    "-fdata-sections -ffunction-sections",
    # Because of how Ninja runs gcc it doesn't know that it has an interactive
    # terminal and disables color output. This makes sure it always outputs color.
    "-fdiagnostics-color=always",
]

COMPILE_FLAGS = [
    # Error on all warnings and enable several useful warnings.
    # -Wall turns on warning for questionable patterns that should be easy to fix.
    # -Wextra adds a few more on top of -Wall that should also be easy to fix.
    # -Wshadow warns about a local symbol shadowing a global symbol.
    # -Wdouble-promotion warns when a value is automatially promoted to a double.
    #   this is especially useful because any code that deals with doubles will
    #   be large and slow and we definitely want to avoid that.
    # -Wformat=2 checks calls to printf & friends to make sure the format specifiers
    #   match the types of the arguments.
    # -Wundef checks for undefined indentifiers in #if macros.
    "-W -Wall -Wextra -Werror -Wshadow -Wdouble-promotion -Wformat=2 -Wundef",
    # Other flags that might be useful:
    # -Wconversion warn about implicit integer conversions
    # Check each function's stack usage against the configured stack size to
    # avoid overflowing the stack.
    f"-Wstack-usage={STACK_SIZE}",
]

DEBUG_COMPILER_FLAGS = [
    "-ggdb3 -Og",
]

RELEASE_COMPILER_FLAGS = [
    # Generally you'd want to use -Os to optimize for size, but since Gemini
    # is super tiny we can optimize for speed instead.
    "-O2"
]

LINK_FLAGS = [
    f"-Wl,--script={LINKER_SCRIPT}",
    # The linker script expects a __stack_size__ symbol to know how much space
    # to set aside for the stack.
    f"-Wl,--defsym=__stack_size__={STACK_SIZE}",
    # Remove unused sections. The compiler generates individual sections for
    # each function and immutable data and the linker can determine if they're
    # unused and cull them.
    "-Wl,--gc-sections",
    # Output a link map. This is helpful when debugging.
    "-Wl,-Map=$builddir/link.map",
]

DEBUG_DEFINES = dict(DEBUG=1)

RELEASE_DEFINES = dict(NDEBUG=1)


# Helper methods


def strigify_paths(paths):
    return [str(path) for path in paths]


def collect_srcs(srcs):
    result = []
    for pattern in srcs:
        if any(_ in pattern for _ in ("*", "[", "?")):
            expanded = pathlib.Path(".").glob(pattern)
            if not expanded:
                print(f"No files match {pattern}")
                sys.exit(1)
            result.extend(expanded)
        else:
            result.append(pathlib.Path(".", pattern))
    return result


def collect_includes(srcs, includes):
    implicit_includes = list(set((path.parent for path in srcs)))
    full_includes = implicit_includes + includes
    return " ".join([f"-I{path}" for path in full_includes])


def collect_defines(defines):
    return " ".join([f"-D{key}={value}" for key, value in defines.items()])


def structy_build(writer, src, **langs_to_dests):
    for lang, dest in langs_to_dests.items():
        output_stem = pathlib.Path(src).stem
        outputs = [pathlib.Path(dest, f"{output_stem}.{lang}")]

        # Include the header as well for C.
        if lang == "c":
            outputs.append(pathlib.Path(dest, f"{output_stem}.h"))

        writer.build(
            strigify_paths(outputs),
            "structy",
            src,
            variables=dict(lang=lang, dest=dest),
        )
        writer.newline()


def check_dependencies():
    if not shutil.which(GCC):
        print(
            f"Requires {GCC}, install from https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads"
        )
        sys.exit(1)

    gcc_version = (
        subprocess.run(
            ["arm-none-eabi-gcc", "-dumpversion"],
            capture_output=True,
            check=True,
            text=True,
        )
        .stdout.strip()
        .split(".")
    )
    gcc_version = tuple(int(x) for x in gcc_version)

    if gcc_version < (10, 2, 0):
        print(
            f"Requires {GCC} >= 10.2.0, install from https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads"
        )
        sys.exit(1)

    if sys.version_info < (3, 9, 0):
        print("Requires python >= 3.9. install from https://www.python.org/downloads/")
        sys.exit(1)


# Buildfile generator

sys.path.insert(1, "third_party/ninja")

import ninja_syntax  # noqa


def generate_build(configuration, run_generators=True):
    srcs = collect_srcs(SRCS)
    includes = collect_includes(srcs, INCLUDES)
    defines = collect_defines(DEFINES)
    compiler_flags = COMMON_FLAGS + COMPILE_FLAGS
    linker_flags = COMMON_FLAGS + LINK_FLAGS

    if configuration == "debug":
        defines += " " + collect_defines(DEBUG_DEFINES)
        compiler_flags += DEBUG_COMPILER_FLAGS
    elif configuration == "release":
        defines += " " + collect_defines(RELEASE_DEFINES)
        compiler_flags += DEBUG_COMPILER_FLAGS

    buildfile_path = pathlib.Path("./build.ninja")
    buildfile = buildfile_path.open("w")
    writer = ninja_syntax.Writer(buildfile)

    # Global variables

    writer.comment("This is generated by configure.py- don't edit it directly!")
    writer.newline()
    writer.variable("builddir", "./build")
    writer.newline()
    writer.variable(
        "cc_flags",
        " ".join(compiler_flags),
    )
    writer.newline()
    writer.variable("cc_includes", includes)
    writer.newline()
    writer.variable("cc_defines", defines)
    writer.newline()
    writer.variable("ld_flags", " ".join(linker_flags))
    writer.newline()

    # Rules for compiling & linking
    writer.rule(
        name="cc",
        command=f"{GCC} $cc_flags $cc_includes $cc_defines -MMD -MT $out -MF $out.d -c $in -o $out",
        depfile="$out.d",
        deps="gcc",
        description="cc $in",
    )
    writer.newline()
    writer.rule(
        name="ld",
        command=f"{GCC} $ld_flags $in -o $out",
        description="link $out",
    )
    writer.newline()
    writer.rule(
        name="elf_to_bin",
        command=f"{OBJCOPY} -O binary $in $out",
        description="Create $out",
    )
    writer.newline()
    writer.rule(
        name="bin_to_uf2",
        command="python3 -m wintertools.bin_to_uf2 $in $out",
        description="Create $out",
    )

    # Rules for structy generation
    writer.rule(
        name="structy",
        command="python3 -m structy_generator -l $lang $in $dest",
        description="Structy ($lang) $in -> $dest",
    )

    # Generic run rules
    writer.newline()
    writer.rule(
        name="runcmd_arg_in", command="$cmd $in $append", description="$desc $in"
    )
    writer.newline()
    writer.rule(
        name="runcmd_arg_out", command="$cmd $out $append", description="$desc $out"
    )
    writer.newline()

    # Builds for source files and elf output
    objects = []

    for src in srcs:
        if src.parts[0] == "build":
            object_path = src.with_suffix(".o")
        else:
            object_path = pathlib.Path("./build") / src.with_suffix(".o")

        writer.build(outputs=str(object_path), rule="cc", inputs=str(src))
        objects.append(object_path)
        writer.newline()

    writer.build(f"build/{PROGRAM}.elf", "ld", strigify_paths(objects))
    writer.newline()

    # Builds for generated files

    # Build info is always generated, even if generators are disabled.
    writer.build(
        "build/generated_build_info.c",
        "runcmd_arg_out",
        "",
        variables=dict(
            cmd=f"python3 -m wintertools.build_info --config {configuration}",
            desc="Generate build info",
        ),
        implicit=["always.phony"],
    )
    writer.newline()

    writer.build("always.phony", "phony")
    writer.newline()

    if run_generators:
        # Structies
        structy_build(
            writer,
            "data/gem_settings.structy",
            c="src/generated",
            py="../factory/libgemini",
            js="../user_guide/docs/scripts",
        )

        structy_build(
            writer,
            "data/gem_monitor_update.structy",
            c="src/generated",
            py="../factory/libgemini",
        )

        # Parameter table
        writer.build(
            "src/generated/gem_lookup_tables.c",
            "runcmd_arg_out",
            "",
            variables=dict(
                cmd="python3 scripts/generate_lookup_tables.py",
                desc="Generate lookup tables",
            ),
            implicit=["scripts/generate_lookup_tables.py"],
        )
        writer.newline()

    # Builds for output format conversion
    writer.build(
        f"build/{PROGRAM}.bin",
        "elf_to_bin",
        f"build/{PROGRAM}.elf",
    )
    writer.newline()

    writer.build(
        f"build/{PROGRAM}.uf2",
        "bin_to_uf2",
        f"build/{PROGRAM}.bin",
    )
    writer.newline()

    # Builds for utilities
    writer.build(
        "size.phony",
        "runcmd_arg_in",
        f"build/{PROGRAM}.elf",
        variables=dict(
            cmd=f"python3 -m wintertools.fw_size --flash-size {FLASH_SIZE} --ram-size {RAM_SIZE}",
            desc="Size",
        ),
    )
    writer.newline()

    writer.build("size", "phony", "size.phony")
    writer.newline()

    format_files = list(pathlib.Path(".").glob("src/**/*.[c,h]"))
    writer.build(
        "format.phony",
        "runcmd_arg_in",
        [str(path) for path in format_files],
        variables=dict(
            cmd="clang-format -i",
            desc="Format",
        ),
    )
    writer.newline()

    writer.build("format", "phony", "format.phony")
    writer.newline()

    # Special reconfigure build
    writer.variable("configure_args", " ".join(sys.argv[1:]))
    writer.newline()
    writer.rule(
        "configure",
        command="python3 configure.py $configure_args",
        generator=True,
        description="Reconfigure with configure.py $configure_args",
    )
    writer.newline()
    writer.build("build.ninja", "configure", implicit=["configure.py"])

    writer.close()


def main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument("--config", choices=["debug", "release"], default="debug")
    parser.add_argument("--skip-checks", action="store_true", default=False)
    parser.add_argument("--no-generators", action="store_true", default=False)

    args = parser.parse_args()

    if not args.skip_checks:
        check_dependencies()

    generate_build(args.config, not args.no_generators)


if __name__ == "__main__":
    main()
