/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    A software implementation of SPI *controller* using just GPIO.
*/

#include <stddef.h>
#include <stdint.h>

#include "wntr_gpio.h"

struct WntrBitBangSPI {
    /* Serial data out from controller - connected to the peripheral's serial data in. */
    struct WntrGPIOPin sdo;
    /* Serial data in to controller - connected to the periphal's serial data out. */
    struct WntrGPIOPin sdi;
    /* Serial clock. */
    struct WntrGPIOPin sck;

    /*
        https://en.wikipedia.org/wiki/Serial_Peripheral_Interface#Clock_polarity_and_phase
    */
    uint8_t clock_polarity;
    uint8_t clock_phase;

    /*
        Internal fields.
    */
    uint32_t _clock_delay;
};

void wntr_bitbang_spi_init(struct WntrBitBangSPI* inst, uint32_t frequency);

void wntr_bitbang_spi_transfer(struct WntrBitBangSPI* inst, const uint8_t* data_out, uint8_t* data_in, size_t len);
void wntr_bitbang_spi_write(struct WntrBitBangSPI* inst, const uint8_t* data_out, size_t len);
void wntr_bitbang_spi_read(struct WntrBitBangSPI* inst, uint8_t* data_in, size_t len);
