/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_bootloader.h"
#include "sam.h"

/*
    The bootloader uses the *last* uin32_t in RAM as the "double tap" magic
    variable. It's picked that way so that the C runtime is unlikely to
    overwrite it during initialization of relocatable data and BSS and
    therefore *should* survive reset.

    This also give the firmware the ability to set this variable directly to
    the magic value before resetting to go straight to the bootloader.

    Reference:
    - https://github.com/adafruit/uf2-samdx1/blob/530fedf5dab77a54e272f0ea1ad3ac0453241f8f/inc/uf2.h#L239-L250
*/
#ifdef SAMD21
static volatile uint32_t* bootloader_flag_ = (uint32_t*)(HMCRAMC0_ADDR + HMCRAMC0_SIZE - 4);
#endif
#ifdef SAMD51
static volatile uint32_t* bootloader_flag_ = (uint32_t*)(HSRAM_ADDR + HSRAM_SIZE - 4);
#endif

/* Taken directly from the uf2-samdx1 source. */
#define BOOTLOADER_MAGIC 0xf01669ef

void wntr_reset_into_bootloader() {
    *bootloader_flag_ = BOOTLOADER_MAGIC;
    NVIC_SystemReset();
}
