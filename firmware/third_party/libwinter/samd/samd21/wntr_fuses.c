/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_fuses.h"
#include "printf.h"

/* 0x02 = 8k, see datasheet section 22.6.5 */
#define BOOTLOADER_BOOTPROT_SIZE 0x02

/* Private forward declarations */
static void print_fuses();

/* Public functions */

void wntr_check_bootprot_fuse() {
    /* Check bootprot and ensure the bootloader is write-protected. */
    if (NVM_USER_ROW->FUSES.bit.BOOTPROT == BOOTLOADER_BOOTPROT_SIZE) {
        /* All good, bootprot is set correctly. */
        printf("Bootloader protection ON (0x%02x)\n", NVM_USER_ROW->FUSES.bit.BOOTPROT);
        return;
    }

    /* bootprot needs to be set. */
    printf("Setting BOOTPROT fuses. Current fuses:\n");
    print_fuses();

    NVM_USER_ROW_Type fuses = *NVM_USER_ROW;
    fuses.FUSES.bit.BOOTPROT = BOOTLOADER_BOOTPROT_SIZE;
    wntr_fuses_write(fuses);

    printf("BOOTPROT set, updated fuses:\n");
    print_fuses();

    /* Reboot. */
    NVIC_SystemReset();
}

void wntr_fuses_write(NVM_USER_ROW_Type fuses) {
    __disable_irq();

    /* Setup NVM for writing and disable cache. */
    uint32_t ctrlb = NVMCTRL->CTRLB.reg;
    NVMCTRL->STATUS.reg |= NVMCTRL_STATUS_MASK;
    NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_CACHEDIS | NVMCTRL_CTRLB_MANW;
    NVMCTRL->ADDR.reg = NVMCTRL_FUSES_BOOTPROT_ADDR / 2;

    /* Erase the row, flush the page cache. */
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_EAR;
    while (NVMCTRL->INTFLAG.bit.READY == 0) {}
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
    while (NVMCTRL->INTFLAG.bit.READY == 0) {}

    /* Write the data - must be done in 16 or 32 bit chunks. */
    ((uint32_t*)NVMCTRL_USER)[0] = ((uint32_t*)&fuses)[0];
    ((uint32_t*)NVMCTRL_USER)[1] = ((uint32_t*)&fuses)[1];

    /* Write the page. */
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WAP;
    while (NVMCTRL->INTFLAG.bit.READY == 0) {}

    /* Restore saved CTRLB value. */
    NVMCTRL->CTRLB.reg = ctrlb;

    __enable_irq();
}

/* Private functions */

static void print_fuses() {
    printf("Fuses: 0x%8x%8x\n", ((uint32_t*)NVMCTRL_USER)[1], ((uint32_t*)NVMCTRL_USER)[0]);
    printf("- BOOTPROT: 0x%x\n", NVM_USER_ROW->FUSES.bit.BOOTPROT);
    printf("- EEPROM: 0x%x\n", NVM_USER_ROW->FUSES.bit.EEPROM);
    printf("- BOD33_LEVEL: 0x%x\n", NVM_USER_ROW->FUSES.bit.BOD33_LEVEL);
    printf("- BOD33_ENABLE: 0x%x\n", NVM_USER_ROW->FUSES.bit.BOD33_ENABLE);
    printf("- BOD33_ACTION: 0x%x\n", NVM_USER_ROW->FUSES.bit.BOD33_ACTION);
    printf("- WDT_ENABLE: 0x%x\n", NVM_USER_ROW->FUSES.bit.WDT_ENABLE);
    printf("- WDT_ALWAYSON: 0x%x\n", NVM_USER_ROW->FUSES.bit.WDT_ALWAYSON);
    printf("- WDT_PERIOD: 0x%x\n", NVM_USER_ROW->FUSES.bit.WDT_PERIOD);
    printf("- WDT_WINDOW: 0x%x\n", NVM_USER_ROW->FUSES.bit.WDT_WINDOW);
    printf("- WDT_EWOFFSET: 0x%x\n", NVM_USER_ROW->FUSES.bit.WDT_EWOFFSET);
    printf("- WDT_WEN: 0x%x\n", NVM_USER_ROW->FUSES.bit.WDT_WEN);
    printf("- BOD33_HYST: 0x%x\n", NVM_USER_ROW->FUSES.bit.BOD33_HYST);
    printf("- LOCK: 0x%x\n", NVM_USER_ROW->FUSES.bit.LOCK);
}
