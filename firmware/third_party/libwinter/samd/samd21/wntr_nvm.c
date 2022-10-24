/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_nvm.h"
#include "sam.h"

#define NVM_MEMORY ((volatile uint16_t*)FLASH_ADDR)

/* Private forward declarations. */

static void erase_row(uint32_t addr);
static void write_page(uint32_t dst, uint8_t* buf, size_t len);

/* Public functions. */

/* These routines are largely adapted from ASF4's hal_nvmctrl.c. */

void wntr_nvm_read(uint32_t src, uint8_t* buf, size_t len) {
    uint32_t nvm_address = src / 2;
    uint32_t i;
    uint16_t data;

    /* Wait until the NVM is free. */
    while (!NVMCTRL->INTFLAG.bit.READY) {};

    /* Clear flags */
    NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;

    /* Check whether byte address is word-aligned*/
    if (src % 2) {
        data = NVM_MEMORY[nvm_address++];  // NOLINT(clang-analyzer-core.NullDereference)
        buf[0] = data >> 8;
        i = 1;
    } else {
        i = 0;
    }

    /* NVM _must_ be accessed as a series of 16-bit words, perform manual copy
     * to ensure alignment */
    while (i < len) {
        data = NVM_MEMORY[nvm_address++];  // NOLINT(clang-analyzer-core.NullDereference)
        buf[i] = (data & 0xFF);
        if (i < (len - 1)) {
            buf[i + 1] = (data >> 8);
        }
        i += 2;
    }
}

void wntr_nvm_write(uint32_t dst, const uint8_t* buf, size_t len) {
    uint8_t tmp_buffer[NVMCTRL_ROW_PAGES][NVMCTRL_PAGE_SIZE];
    uint32_t row_start_addr, row_end_addr;
    uint32_t i, j, k;
    uint32_t wr_start_addr = dst;

    do {
        row_start_addr = wr_start_addr & ~((NVMCTRL_PAGE_SIZE * NVMCTRL_ROW_PAGES) - 1);
        row_end_addr = row_start_addr + NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE - 1;

        /* store the erase data into temp buffer before write */
        for (i = 0; i < NVMCTRL_ROW_PAGES; i++) {
            wntr_nvm_read(row_start_addr + i * NVMCTRL_PAGE_SIZE, tmp_buffer[i], NVMCTRL_PAGE_SIZE);
        }

        /* temp buffer update */
        j = (wr_start_addr - row_start_addr) / NVMCTRL_PAGE_SIZE;
        k = wr_start_addr - row_start_addr - j * NVMCTRL_PAGE_SIZE;
        while ((wr_start_addr <= row_end_addr) && (len > 0)) {
            tmp_buffer[j][k] = *buf;
            k = (k + 1) % NVMCTRL_PAGE_SIZE;
            if (0 == k) {
                j++;
            }
            wr_start_addr++;
            buf++;
            len--;
        }

        /* erase row before write */
        erase_row(row_start_addr);

        /* write buffer to flash */
        for (i = 0; i < NVMCTRL_ROW_PAGES; i++) {
            write_page(row_start_addr + i * NVMCTRL_PAGE_SIZE, tmp_buffer[i], NVMCTRL_PAGE_SIZE);
        }

    } while (row_end_addr < (wr_start_addr + len - 1));
}

/* Private functions. */

static void erase_row(uint32_t addr) {
    /* Wait until the NVM is free. */
    while (!NVMCTRL->INTFLAG.bit.READY) {};

    /* Clear flags */
    NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;

    /* Set address and send erase command. */
    NVMCTRL->ADDR.reg = addr / 2;
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_ER | NVMCTRL_CTRLA_CMDEX_KEY;
}

static void write_page(uint32_t dst, uint8_t* buf, size_t len) {
    uint32_t nvm_address = dst / 2;
    uint16_t i, data;

    /* Wait until the NVM is free. */
    while (!NVMCTRL->INTFLAG.bit.READY) {};

    /* Clear the page buffer. */
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_PBC | NVMCTRL_CTRLA_CMDEX_KEY;
    while (!NVMCTRL->INTFLAG.bit.READY) {};

    /* Clear flags */
    NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;

    for (i = 0; i < len; i += 2) {
        data = buf[i];
        if (i < NVMCTRL_PAGE_SIZE - 1) {
            data |= (buf[i + 1] << 8);
        }
        NVM_MEMORY[nvm_address++] = data;  // NOLINT(clang-analyzer-core.NullDereference)
    }

    /* Send the write page command to finish the write. */
    while (!NVMCTRL->INTFLAG.bit.READY) {};
    NVMCTRL->ADDR.reg = dst / 2;
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_WP | NVMCTRL_CTRLA_CMDEX_KEY;
}
