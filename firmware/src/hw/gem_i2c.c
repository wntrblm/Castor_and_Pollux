/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_i2c.h"
#include "gem_config.h"
#include "printf.h"
#include "wntr_gpio.h"

#define BUSSTATE_UNKNOWN 0
#define BUSSTATE_IDLE 1
#define BUSSTATE_OWNER 2
#define BUSSTATE_BUSY 3

void gem_i2c_init(const struct GemI2CConfig* cfg) {
    /* Enable the APB clock for SERCOM. */
    PM->APBCMASK.reg |= cfg->apbcmask;

    /* Enable GCLK1 for the SERCOM */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | cfg->gclk | cfg->clkctrl_id;

    /* Wait for bus synchronization. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    /* Reset the SERCOM. */
    cfg->sercom->I2CM.CTRLA.bit.ENABLE = 0;
    while (cfg->sercom->I2CM.SYNCBUSY.bit.ENABLE) {};
    cfg->sercom->I2CM.CTRLA.bit.SWRST = 1;
    while (cfg->sercom->I2CM.SYNCBUSY.bit.SWRST || cfg->sercom->I2CM.SYNCBUSY.bit.ENABLE) {};

    /* Configure pins for the correct function. */
    WntrGPIOPin_configure_alt(cfg->pad0_pin);
    WntrGPIOPin_configure_alt(cfg->pad1_pin);

    /* Configure SERCOM for i2c master. */
    cfg->sercom->I2CM.CTRLA.bit.SWRST = 1;
    while (cfg->sercom->I2CM.SYNCBUSY.bit.SWRST || cfg->sercom->I2CM.CTRLA.bit.SWRST) {};

    cfg->sercom->I2CM.CTRLA.reg = (
        // 0 is standard/fast mode 100 & 400kHz
        SERCOM_I2CM_CTRLA_SPEED(0) |
        // Hold SDA low for 300-600ns
        SERCOM_I2CM_CTRLA_SDAHOLD(0) |
        // work as master
        SERCOM_I2CM_CTRLA_MODE_I2C_MASTER);

    /* Enable smart mode */
    cfg->sercom->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN | SERCOM_I2CM_CTRLB_QCEN;
    while (cfg->sercom->I2CM.SYNCBUSY.bit.SYSOP) {};

    /* Set baudrate.
        this is from Arduino:
            https://github.com/arduino/ArduinoCore-samd/blob/master/cores/arduino/SERCOM.cpp#L461
    */
    uint32_t baudrate = cfg->baudrate;      // Hz
    uint32_t clock_speed = cfg->gclk_freq;  // Hz
    uint32_t rise_time = cfg->rise_time;    // ns
    cfg->sercom->I2CM.BAUD.bit.BAUD =
        clock_speed / (2 * baudrate) - 5 - (((clock_speed / 1000000) * rise_time) / (2 * 1000));
    while (cfg->sercom->I2CM.SYNCBUSY.bit.SYSOP) {};

    /* Enable the SERCOM. */
    cfg->sercom->I2CM.CTRLA.bit.ENABLE = 1;
    while (cfg->sercom->I2CM.SYNCBUSY.bit.ENABLE) {};

    /* Put the bus into the idle state. */
    cfg->sercom->I2CM.STATUS.bit.BUSSTATE = BUSSTATE_IDLE;
    while (cfg->sercom->I2CM.SYNCBUSY.bit.SYSOP) {};
}

enum GemI2CResult gem_i2c_write(const struct GemI2CConfig* cfg, uint8_t address, uint8_t* data, size_t len) {
    /* Before trying to write, check to see if the bus is busy, if it is,
       bail.
    */
    if (cfg->sercom->I2CM.STATUS.bit.BUSSTATE == BUSSTATE_BUSY) {
        return GEM_I2C_RESULT_ERR_BUSSTATE;
    }

    /* Address + write flag. */
    cfg->sercom->I2CM.ADDR.bit.ADDR = (address << 0x1ul) | 0;

    /* This can hang forever, so put a timeout on it. */
    size_t w = 0;
    for (; w < cfg->wait_timeout; w++) {
        if (cfg->sercom->I2CM.INTFLAG.bit.MB) {
            break;
        }
    }

#ifdef DEBUG
    if (w == cfg->wait_timeout) {
        printf("I2C timeout hit!");
    }
#endif

    /* Check for loss of bus or NACK - in either case we can't continue. */
    if (cfg->sercom->I2CM.STATUS.bit.BUSSTATE != BUSSTATE_OWNER) {
        return GEM_I2C_RESULT_ERR_BUSSTATE;
    }
    if (cfg->sercom->I2CM.STATUS.bit.RXNACK) {
        return GEM_I2C_RESULT_ERR_ADDR_NACK;
    }

    /* Send data bytes. */
    for (size_t i = 0; i < len; i++) {
        /* Send data and wait for TX complete. */
        cfg->sercom->I2CM.DATA.bit.DATA = data[i];

        while (!cfg->sercom->I2CM.INTFLAG.bit.MB) {
            /* Check for loss of arbitration or a bus error. We can't continue if those happen. */
            /* BUSERR is set in addition to ARBLOST if arbitration is lost, so just check that one. */
            if (cfg->sercom->I2CM.STATUS.bit.BUSERR) {
                return GEM_I2C_RESULT_ERR_BUSERR;
            }
        }

        /* If a nack is received we can not continue sending data. */
        if (cfg->sercom->I2CM.STATUS.bit.RXNACK) {
            return GEM_I2C_RESULT_ERR_DATA_NACK;
        }
    }

    /* Send STOP command. */
    cfg->sercom->I2CM.CTRLB.bit.CMD = 3;
    while (cfg->sercom->I2CM.SYNCBUSY.bit.SYSOP) {};

    return GEM_I2C_RESULT_SUCCESS;
}
