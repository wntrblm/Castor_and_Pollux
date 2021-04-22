/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_mtb.h"
#include "assert.h"
#include "sam.h"

#ifndef WNTR_MTB_SIZE
/* The number of entries the MTB packet buffer can hold at a time. */
#define WNTR_MTB_SIZE 128
#endif

/* The length, in bytes, of the MTB packet buffer. */
#define MTB_LENGTH (WNTR_MTB_SIZE * sizeof(uint32_t))

#define IS_POWER_OF_TWO(x) ((x > 1) & !(x & (x - 1)))

static_assert(IS_POWER_OF_TWO(MTB_LENGTH), "WNTR_MTB_SIZE * 4 must be a power of 2.");

/*
    This data is the circular buffer of MTB packets.

    It must be aligned to the size of the buffer: this is because of how the
    MTB addresses this memory and how it wraps around.

    First, the addressing. It uses a 32-bit pointer but doesn't use the lower
    three bits- because it addresses & writes *two* 32-bit values at a time so
    it only needs to increment the pointer by 8 for each packet. For example,
    it would write this sequence for three packets:

    > 0b00??? (0x00) = [packet 1]
    > 0b01??? (0x08) = [packet 2]
    > 0b10??? (0x10) = [packet 3]

    This addressing scheme necessitates aligning on 8-bytes, but the
    wrap-around scheme adds the additional align-to-size requirement. When the
    MTB reaches the end of the buffer, it can only clear one bit to wrap the
    buffer back around. For example, if the length is 16 entries and the buffer
    is located at 0xCAFE00, the wrap-around would work like this:

    > 0xCAFE00 = [packet 1]
    > ...
    > 0xCAFE38 = [packet 7]
    > 0xCAFE40 = [packet 8]
    Wrap-around, bit 7 is cleared. ptr = 0xCAFE40 & ~(0x40)
    > 0xCAFE00 = [packet 9]

    So that means that the size of the MTB must be a power of two and it must
    be aligned on a boundary that's equal to the size of the MTB because the
    wrap-around can only clear one bit.

    Reference:
    * https://developer.arm.com/documentation/ddi0486/b/functional-description/operation/mtb-execution-trace-packet-format?lang=en
    * https://learn.adafruit.com/debugging-the-samd21-with-gdb/micro-trace-buffer
*/
#ifdef WNTR_ENABLE_MTB
__attribute__((__aligned__(WNTR_MTB_SIZE * sizeof(uint32_t)))) uint32_t mtb[WNTR_MTB_SIZE];
#endif

#ifndef WNTR_ENABLE_MTB

void wntr_mtb_init() {}
void wntr_mtb_enable() {}
void wntr_mtb_disable() {}

#else

void wntr_mtb_init() {
    /*
        Configure the MTB to write to the RAM set aside for packets.

        Bits 0-1: Reserved
        Bit 2: WRAP, not writable
        Bits 3-32: POINTER, offset from MTB->BASE where the next trace will be
            written. Note that for most systems, BASE is simply the start of
            SRAM.

        Reference:
        * https://developer.arm.com/documentation/ddi0486/b/programmers-model/register-descriptions/position-register?lang=en
    */
    uint32_t mtb_offset_addr = (uint32_t)(mtb - MTB->BASE.reg);
    MTB->POSITION.reg = mtb_offset_addr & 0xFFFFFFF8;

    /*
        Configure the MTB's wrap around address.

        The FLOW.WATERMARK pointer is in the same format as the one in
        POSITION.POINTER and is the last address that the MTB should write to.
        Once it gets there, it'll wrap back around.

        Bit 0: AUTOHALT, not used.
        Bit 1: AUTOSTOP, not used.
        Bit 2: Reserved
        Bits 3-32: WATERMARK, offset from MTB->BASE where the next packet will
            be written.
    */
    MTB->FLOW.reg = (mtb_offset_addr + MTB_LENGTH) & 0xFFFFFFF8;

    /*
        Configure the wrap around behavior and enable the MTB.

        MASK must be set according to the size of the MTB packet buffer- it
        specifies the *most significant bit* of POSITION.POINTER that can be
        updated by the MTB. It's a little roundabout, though, because of the
        addressing scheme. MASK(n) specifies bit (n + 3) in the full POSITION
        register.

        For example, if MASK = 3:

        > POSITION = 0b???? ???? ??00 0000
        >                          ^   ^^^
        > MASK(3) ─────────────────┘    |
        > Unused bits ──────────────────┘


        With MASK(3) the MTB can store up to 16 packets. This is because the
        total number of bytes the MTB can address is 2^(MASK + 4) = 128 bytes.
        Since there are eight bytes in each packet that gives a total of 16
        packets.

        This function works backwards and figures out the mask for a given MTB
        buffer size by counting the number of trailing zeros in the buffer
        size. Continuing the same example, there are 7 trailing zeros in the
        buffer size (64 = 0b1000 0000). It subtracts one bit because the MASK is
        the highest bit that can change (it's the same concept as the difference
        between the length of an array vs the highest index you can access).
        It then subtracts the 3 unused bits in the POSITION register, so,
        MASK is 7 - 1 - 3 = 3.

        Reference
        * https://developer.arm.com/documentation/ddi0486/b/programmers-model/register-descriptions/master-register?lang=en
    */
    uint32_t mask = __builtin_ctz(MTB_LENGTH) - 4;
    MTB->MASTER.bit.MASK = mask;
    MTB->MASTER.bit.EN = 1;
}

void wntr_mtb_disable() { MTB->MASTER.bit.EN = 0; }

void wntr_mtb_enable() { MTB->MASTER.bit.EN = 1; }

#endif
