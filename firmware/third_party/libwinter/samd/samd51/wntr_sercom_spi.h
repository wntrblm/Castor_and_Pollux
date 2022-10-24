/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* SERCOM SPI driver for SAMD51 */

#include "sam.h"
#include "wntr_gpio.h"
#include "wntr_sercom.h"
#include <stddef.h>

struct WntrSERCOMSPI {
    SercomSpi* sercom;
    uint8_t dopo : 2;
    uint8_t phase : 1;
    uint8_t polarity : 1;
    struct WntrGPIOPin sdo;
    uint8_t sdo_alt;
    struct WntrGPIOPin sck;
    uint8_t sck_alt;
    struct WntrGPIOPin cs;
    /* Leave zero to disable hardware CS */
    uint8_t cs_alt;
};

void WntrSERCOMSPI_init(const struct WntrSERCOMSPI* inst);
void WntrSERCOMSPI_write(const struct WntrSERCOMSPI* inst, const uint8_t* data, size_t len);
