OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

/* Memory Spaces Definitions */
MEMORY
{
    /* For the uf2-samdx1 bootloader. */
    bootloader (rx) : ORIGIN = 0x00000000, LENGTH = 0x00002000
    /* Space for the firmware, minus the size of the bootloader and the nvm region. */
    rom      (rx)   : ORIGIN = 0x00002000, LENGTH = 0x00040000 - 0x2000 - 1024
    /* Space for settings and calibration data saved to NVM/Flash.
       Note: this is set to read-only, but it can be written to using the
       processor-specific NVM peripheral.
    */
    nvm      (r)    : ORIGIN = 0x00040000 - 1024, LENGTH = 1024
    /* Finally, RAM. */
    ram      (rwx)  : ORIGIN = 0x20000000, LENGTH = 0x00008000
}

/* The stack size for C/C++ applications.
   This can be adjusted as needed, but 0x2000 (8k) is a good starting point.
   You can track stack usage using the -Wstack-usage gcc flag.
*/
STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x2000;

/* Bootloader double tap variable location.
   This isn't used by Gemini, but it allows the firmware to reset into
   the bootloader by setting this to the magic number 0xf01669ef

   (see https://github.com/adafruit/uf2-samdx1/blob/2d027f3c941d689a99d68620d5843aaae4100e9c/inc/uf2.h#L249)
 */
_bootloader_dbl_tap = ORIGIN(ram) + LENGTH(ram) - 4;

/* Pointers to the NVM region.
   This is used by the settings module to know where to load and
   save settings.
*/
_nvm_settings_base_address = ORIGIN(nvm);
_nvm_settings_length = LENGTH(nvm) / 2;

/* Pointers to the calibration/look-up table NVM region.
   Gemini uses half of the NVM region to store the look-up table
   for translating ADC -> frequency/dac codes.
*/
_nvm_lut_base_address = ORIGIN(nvm) + LENGTH(nvm) / 2;
_nvm_lut_length = LENGTH(nvm) / 2;

/* Section Definitions */
SECTIONS
{
    .text :
    {
        /* The reset vectors must be in the first section of ROM.
           (well, technically they don't because we use relocatable
           vectors, but it's still good practice).
        */
        . = ALIGN(4);
        _sfixed = .;
        KEEP(*(.vectors .vectors.*))

        /* For code and read-only data. */
        *(.text .text.* .gnu.linkonce.t.*)
        *(.rodata .rodata* .gnu.linkonce.r.*)

        /* This is used to glue ARM & Thumb instructions.
           Because Gemini using a Cortex-M0+ it only uses Thumb instructions
           so this isn't strictly needed, however, it's useful to keep in
           just in case you switch to another processor.

           (see https://gcc.gnu.org/legacy-ml/gcc-help/2009-03/msg00306.html)
        */
        *(.glue_7t) *(.glue_7)

        /* For ARM exception/stack unwinding.
           see https://stackoverflow.com/a/57463515
        */
        *(.ARM.extab* .gnu.linkonce.armextab.*)

        /* The following sections support the C/C++ runtime.

           see:
            * https://developer.arm.com/documentation/dui0475/h/the-arm-c-and-c---libraries/c---initialization--construction-and-destruction
            * https://stackoverflow.com/questions/15265295/understanding-the-libc-init-array
            * https://refspecs.linuxfoundation.org/elf/gabi4+/ch5.dynamic.html#init_fini
            * https://gcc.gnu.org/onlinedocs/gccint/Initialization.html
        */

        /* Static constructors */
        . = ALIGN(4);
        KEEP(*(.init))
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        /* Constructors & vtable management.
           Called by the complier-generated __main function
           when using a C++ runtime.
        */
        . = ALIGN(4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        /* Destructors, static desctructors, and atexit()
           Note that these aren't ever really called because "exit"
           is generally a reset or power off.
        */
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))
        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;

        . = ALIGN(4);
        _efixed = .;            /* End of text section */
    } > rom


    /* For ARM exception/stack unwinding.
        see https://stackoverflow.com/a/57463515
    */
    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > rom
    PROVIDE_HIDDEN (__exidx_end = .);

    . = ALIGN(4);
    _etext = .;

    /* Initilized data/variables that need to be copied from flash to RAM. */
    .relocate : AT (_etext)
    {
        . = ALIGN(4);
        _srelocate = .;
        *(.data .data.*);
        . = ALIGN(4);
        _erelocate = .;
    } > ram

    /* Reserved space for uninitialized data/variables.

       This is all zeroed out during startup.
       The ELF specification calls this "bss".

       See:
        * https://en.wikipedia.org/wiki/.bss
    */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = .;
        _szero = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        _ezero = .;
    } > ram

    /* Stack section in RAM, it must be 8-byte aligned. */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        _sstack = .;
        . = . + STACK_SIZE;
        . = ALIGN(8);
        _estack = .;
    } > ram

    /* .end marks the end of used RAM and the beginning of the heap. */
    . = ALIGN(4);
    _end = . ;
}
