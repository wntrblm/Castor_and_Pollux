/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "printf.h"
#include "sam.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define NVMCTRL_BLOCK_NUM_PAGES (NVMCTRL_BLOCK_SIZE / NVMCTRL_PAGE_SIZE)
#define NVMCTRL_BLOCK_ADDR_MASK (~(NVMCTRL_BLOCK_SIZE - 1))

static void nvm_erase_block_(uint8_t* dst) {
    while (NVMCTRL->STATUS.bit.READY == 0) {};

    NVMCTRL->ADDR.reg = (uint32_t)(dst);
    NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_EB;

    while (NVMCTRL->STATUS.bit.READY == 0) {};
    NVMCTRL->INTFLAG.reg = NVMCTRL_INTFLAG_DONE;
}

static void nvm_write_page_(uint8_t* dst, uint8_t* src) {
    NVMCTRL->CTRLA.bit.WMODE = NVMCTRL_CTRLA_WMODE_MAN;
    while (NVMCTRL->STATUS.bit.READY == 0) {};

    // clear the page buffer
    NVMCTRL->ADDR.reg = (uint32_t)(dst);
    NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_PBC;
    while (NVMCTRL->STATUS.bit.READY == 0) {};
    NVMCTRL->INTFLAG.reg = NVMCTRL_INTFLAG_DONE;

    // writes must be done 32 bits at a time.
    uint32_t* dst32 = (uint32_t*)(dst);
    uint32_t* src32 = (uint32_t*)(src);

    for (size_t i = 0; i < (NVMCTRL_PAGE_SIZE / sizeof(uint32_t)); i++) { dst32[i] = src32[i]; }

    // write the whole pagebuffer to nvm
    NVMCTRL->ADDR.reg = (uint32_t)(dst);
    NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_WP;
    while (NVMCTRL->STATUS.bit.READY == 0) {};
    NVMCTRL->INTFLAG.reg = NVMCTRL_INTFLAG_DONE;
}

void wntr_nvm_write(uint8_t* dst, uint8_t* src, size_t src_len) {
    // Disable caches while reading and writing at the same time.
    NVMCTRL->CTRLA.bit.CACHEDIS0 = true;
    NVMCTRL->CTRLA.bit.CACHEDIS1 = true;

    uint8_t block_buf[NVMCTRL_BLOCK_SIZE];
    size_t idx = 0;

    while (idx < src_len) {
        // Mask off the block size to figure out which block this is in.
        uint8_t* block_start = (dst + idx);
        block_start = (uint8_t*)((uint32_t)(block_start)&NVMCTRL_BLOCK_ADDR_MASK);

        printf("NVM: writing block 0x%x\n", block_start);

        // copy the contents of the block into our temporary buffer so we've
        // got a copy we can modify while erasing the block
        memcpy(block_buf, block_start, NVMCTRL_BLOCK_SIZE);

        // update our copy of the buffer with the new data, being careful
        // not to write past the block boundary.
        uint32_t block_write_offset = (dst + idx) - block_start;
        size_t block_write_size = src_len - idx;
        uint32_t block_write_end = block_write_offset + block_write_size;

        if (block_write_end > NVMCTRL_BLOCK_SIZE) {
            block_write_size = block_write_end - NVMCTRL_BLOCK_SIZE;
        }

        memcpy(block_buf + block_write_offset, src + idx, block_write_size);

        idx += block_write_size;

        // Erase the block, then write all of its pages
        nvm_erase_block_(block_start);

        for (size_t i = 0; i < NVMCTRL_BLOCK_NUM_PAGES; i++) {
            size_t page_offset = (i * NVMCTRL_PAGE_SIZE);
            nvm_write_page_(block_start + page_offset, block_buf + page_offset);
        }
    }
}
