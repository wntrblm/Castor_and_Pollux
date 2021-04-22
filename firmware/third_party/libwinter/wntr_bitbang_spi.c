/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_bitbang_spi.h"
#include "wntr_delay.h"
#include "wntr_platform.h"

/*
    Private functions
*/
static bool is_valid_pin_(struct WntrGPIOPin pin) { return !(pin.port == 0 && pin.pin == 0); }

/*
    Public functions
*/

void wntr_bitbang_spi_init(struct WntrBitBangSPI* inst, uint32_t frequency) {
    /* Configure pins */
    if (is_valid_pin_(inst->sdo)) {
        WntrGPIOPin_set_as_output(inst->sdo);
    }
    if (is_valid_pin_(inst->sdi)) {
        WntrGPIOPin_set_as_input(inst->sdi, false);
    }

    WntrGPIOPin_set_as_output(inst->sck);

    /*
        Drive the SCK line to the appropriate idle state.
    */
    WntrGPIOPin_set(inst->sck, inst->clock_polarity);

    /*
        Calculate delay for the given SPI frequency. Since each byte involves
        two delays (one for the leading clock, one for the trailing clock),
        divide it by two.

        TODO: This needs to be tested with actual hardware.
    */
    inst->_clock_delay = SystemCoreClock / frequency / 2;
}

void wntr_bitbang_spi_transfer(struct WntrBitBangSPI* inst, const uint8_t* data_out, uint8_t* data_in, size_t len) {
    const bool clock_leading = inst->clock_polarity == 0 ? true : false;
    const bool clock_trailing = !clock_leading;

    for (size_t i = 0; i < len; i++) {
        uint8_t out_byte = 0;
        uint8_t in_byte = 0;

        if (data_out != NULL) {
            out_byte = data_out[i];
        }

        for (uint8_t bitmask = 0x80; bitmask; bitmask >>= 1) {
            bool out_bit = (out_byte & bitmask);
            WntrGPIOPin_set(inst->sdo, out_bit);

            if (inst->clock_phase == 0) {
                wntr_delay_us(inst->_clock_delay);
                WntrGPIOPin_set(inst->sck, clock_leading);
            } else {
                WntrGPIOPin_set(inst->sck, clock_leading);
                wntr_delay_us(inst->_clock_delay);
            }

            if (is_valid_pin_(inst->sdi) && WntrGPIOPin_get(inst->sdi)) {
                in_byte |= bitmask;
            }

            if (inst->clock_phase == 0) {
                wntr_delay_us(inst->_clock_delay);
                WntrGPIOPin_set(inst->sck, clock_trailing);
            } else {
                WntrGPIOPin_set(inst->sck, clock_trailing);
                wntr_delay_us(inst->_clock_delay);
            }
        }

        if (data_in != NULL) {
            data_in[i] = in_byte;
        }
    }
}

void wntr_bitbang_spi_write(struct WntrBitBangSPI* inst, const uint8_t* data_out, size_t len) {
    return wntr_bitbang_spi_transfer(inst, data_out, NULL, len);
}

void wntr_bitbang_spi_read(struct WntrBitBangSPI* inst, uint8_t* data_in, size_t len) {
    return wntr_bitbang_spi_transfer(inst, NULL, data_in, len);
}
