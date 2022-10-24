/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_sercom_spi.h"
#include "wntr_delay.h"

/* 12 MHz clock for SPI */
#define SERCOM_SPI_GCLK GCLK_PCHCTRL_GEN_GCLK4;

void WntrSERCOMSPI_init(const struct WntrSERCOMSPI* inst) {
    /* Enable clocks */
    wntr_sercom_init_clock((Sercom*)inst->sercom, GCLK_PCHCTRL_GEN_GCLK4);

    /* Reset and configure */
    inst->sercom->CTRLA.bit.ENABLE = 0;
    while (inst->sercom->SYNCBUSY.bit.ENABLE) {};

    inst->sercom->CTRLA.bit.SWRST = 1;
    while (inst->sercom->SYNCBUSY.bit.SWRST || inst->sercom->CTRLA.bit.SWRST) {};

    /* Setup SPI controller and mode (0x3 = controller) */
    inst->sercom->CTRLA.reg = (SERCOM_SPI_CTRLA_MODE(0x3) | SERCOM_SPI_CTRLA_DOPO(inst->dopo));

    if (inst->phase) {
        inst->sercom->CTRLA.bit.CPHA = 1;
    }
    if (inst->polarity) {
        inst->sercom->CTRLA.bit.CPOL = 1;
    }

    /* Set baud to max (GCLK / 2) 6 MHz */
    inst->sercom->BAUD.reg = SERCOM_SPI_BAUD_BAUD(16);

    /* Configure pins for the correct function. */
    WntrGPIOPin_set_as_output(inst->sdo);
    WntrGPIOPin_configure_alt(inst->sdo, inst->sdo_alt);
    WntrGPIOPin_set_as_output(inst->sck);
    WntrGPIOPin_configure_alt(inst->sck, inst->sck_alt);

    if (!(inst->cs.port == 0 && inst->cs.pin == 0)) {
        WntrGPIOPin_set_as_output(inst->cs);
        WntrGPIOPin_set(inst->cs, true);
        if (inst->cs_alt != 0) {
            WntrGPIOPin_configure_alt(inst->cs, inst->cs_alt);
            inst->sercom->CTRLB.bit.MSSEN = 1;
            while (inst->sercom->SYNCBUSY.bit.CTRLB) {};
        }
    }

    /* Finally, enable it! */
    inst->sercom->CTRLA.bit.ENABLE = 1;
    while (inst->sercom->SYNCBUSY.bit.ENABLE) {};
}

void WntrSERCOMSPI_write(const struct WntrSERCOMSPI* inst, const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        while (!inst->sercom->INTFLAG.bit.DRE) {}
        inst->sercom->DATA.bit.DATA = data[i];
    }
    while (!inst->sercom->INTFLAG.bit.TXC) {}
}
