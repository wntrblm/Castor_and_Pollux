#include "gem_spi.h"
#include "gem_config.h"
#include "gem_gpio.h"
#include "sam.h"

void gem_spi_init() { /* Enable the APB clock for SERCOM. */
    PM->APBCMASK.reg |= GEM_SPI_SERCOM_APBCMASK;

    /* Enable GCLK1 for the SERCOM */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GEM_SPI_GCLK | GEM_SPI_GCLK_CLKCTRL_ID;

    /* Wait for bus synchronization. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    /* Configure pins for the correct function. */
    gem_gpio_set_mux(GEM_SPI_SCK_PORT, GEM_SPI_SCK_PIN, GEM_SPI_SCK_PIN_FUNC);
    gem_gpio_set_mux(GEM_SPI_SDO_PORT, GEM_SPI_SDO_PIN, GEM_SPI_SDO_PIN_FUNC);

    /* Setup SPI controller. */
    GEM_SPI_SERCOM->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_MODE_SPI_MASTER | SERCOM_SPI_CTRLA_DOPO(GEM_SPI_DOPO);

    if (GEM_SPI_POLARITY)
        GEM_SPI_SERCOM->SPI.CTRLA.bit.CPOL = 1;
    if (GEM_SPI_PHASE)
        GEM_SPI_SERCOM->SPI.CTRLA.bit.CPHA = 1;

    /* Set baud */
    uint32_t baudrate = GEM_SPI_BAUD;
    GEM_SPI_SERCOM->SPI.BAUD.reg = GEM_SPI_GCLK_FREQ / (2 * baudrate) - 1;

    /* Enable the SERCOM. */
    GEM_SPI_SERCOM->SPI.CTRLA.bit.ENABLE = 1;
    while (GEM_SPI_SERCOM->SPI.SYNCBUSY.bit.ENABLE) {};
}

void gem_spi_write(const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        while (!GEM_SPI_SERCOM->SPI.INTFLAG.bit.DRE) {}
        GEM_SPI_SERCOM->SPI.DATA.reg = data[i];
    }
}