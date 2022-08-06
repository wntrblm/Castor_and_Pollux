/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_spi.h"

void gem_spi_init(const struct GemSPIConfig* spi) { /* Enable the APB clock for SERCOM. */
    PM->APBCMASK.reg |= spi->apbcmask;

    /* Enable GCLK1 for the SERCOM */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | spi->gclk | spi->clkctrl_id;

    /* Wait for bus synchronization. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    /* Reset the SERCOM. */
    spi->sercom->SPI.CTRLA.bit.ENABLE = 0;
    while (spi->sercom->SPI.SYNCBUSY.bit.ENABLE) {};
    spi->sercom->SPI.CTRLA.bit.SWRST = 1;
    while (spi->sercom->SPI.SYNCBUSY.bit.SWRST || spi->sercom->SPI.SYNCBUSY.bit.ENABLE) {};

    /* Configure pins for the correct function. */
    WntrGPIOPin_configure_alt(spi->sck_pin);
    WntrGPIOPin_configure_alt(spi->sdo_pin);

    /* Setup SPI controller. */
    spi->sercom->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_MODE_SPI_MASTER | SERCOM_SPI_CTRLA_DOPO(spi->dopo);

    if (spi->polarity)
        spi->sercom->SPI.CTRLA.bit.CPOL = 1;
    if (spi->phase)
        spi->sercom->SPI.CTRLA.bit.CPHA = 1;

    /* Set baud */
    uint32_t baudrate = spi->baud;
    if (baudrate == spi->gclk_freq) {
        spi->sercom->SPI.BAUD.reg = 0x1;
    } else {
        spi->sercom->SPI.BAUD.reg = spi->gclk_freq / (2 * baudrate) - 1;
    }

    /* Enable the SERCOM. */
    spi->sercom->SPI.CTRLA.bit.ENABLE = 1;
    while (spi->sercom->SPI.SYNCBUSY.bit.ENABLE) {};
}

void gem_spi_write(const struct GemSPIConfig* spi, const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        while (!spi->sercom->SPI.INTFLAG.bit.DRE) {}
        spi->sercom->SPI.DATA.reg = data[i];
    }
}
