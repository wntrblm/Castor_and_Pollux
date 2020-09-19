#include "gem_i2c.h"
#include "gem_config.h"
#include "gem_gpio.h"

#define BUSSTATE_UNKNOWN 0
#define BUSSTATE_IDLE 1
#define BUSSTATE_OWNER 2
#define BUSSTATE_BUSY 3

void gem_i2c_init() {
    /* Enable the APB clock for SERCOM. */
    PM->APBCMASK.reg |= GEM_I2C_SERCOM_APBCMASK;

    /* Enable GCLK1 for the SERCOM */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GEM_I2C_GCLK | GEM_I2C_GCLK_CLKCTRL_ID;

    /* Wait for bus synchronization. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    /* Reset the SERCOM. */
	GEM_I2C_SERCOM->I2CM.CTRLA.bit.ENABLE = 0;
	while(GEM_I2C_SERCOM->I2CM.SYNCBUSY.bit.ENABLE) {};
	GEM_I2C_SERCOM->I2CM.CTRLA.bit.SWRST = 1;
	while(GEM_I2C_SERCOM->I2CM.SYNCBUSY.bit.SWRST || GEM_I2C_SERCOM->I2CM.SYNCBUSY.bit.ENABLE) {};

    /* Configure pins for the correct function. */
    gem_gpio_set_mux(GEM_I2C_PAD_0_PORT, GEM_I2C_PAD_0_PIN, GEM_I2C_PAD_0_PIN_FUNC);
    gem_gpio_set_mux(GEM_I2C_PAD_1_PORT, GEM_I2C_PAD_1_PIN, GEM_I2C_PAD_1_PIN_FUNC);

    /* Configure SERCOM for i2c master. */
    GEM_I2C_SERCOM->I2CM.CTRLA.bit.SWRST = 1;
    while (GEM_I2C_SERCOM->I2CM.SYNCBUSY.bit.SWRST || GEM_I2C_SERCOM->I2CM.CTRLA.bit.SWRST) {};

    GEM_I2C_SERCOM->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_SPEED(0) |        // 0 is standard/fast mode 100 & 400kHz
                                     SERCOM_I2CM_CTRLA_SDAHOLD(0) |      // Hold SDA low for 300-600ns
                                     SERCOM_I2CM_CTRLA_MODE_I2C_MASTER;  // work as master

    /* Enable smart mode */
    GEM_I2C_SERCOM->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN | SERCOM_I2CM_CTRLB_QCEN;
    while (GEM_I2C_SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};

    /* Set baudrate.
        this is from Arduino:
            https://github.com/arduino/ArduinoCore-samd/blob/master/cores/arduino/SERCOM.cpp#L461
    */
    uint32_t baudrate = GEM_I2C_BAUDRATE;      // Hz
    uint32_t clock_speed = GEM_I2C_GCLK_FREQ;  // Hz
    uint32_t rise_time = GEM_I2C_RISE_TIME;    // ns
    GEM_I2C_SERCOM->I2CM.BAUD.bit.BAUD =
        clock_speed / (2 * baudrate) - 5 - (((clock_speed / 1000000) * rise_time) / (2 * 1000));
    while (GEM_I2C_SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};

    /* Enable the SERCOM. */
    GEM_I2C_SERCOM->I2CM.CTRLA.bit.ENABLE = 1;
    while (GEM_I2C_SERCOM->I2CM.SYNCBUSY.bit.ENABLE) {};

    /* Put the bus into the idle state. */
    GEM_I2C_SERCOM->I2CM.STATUS.bit.BUSSTATE = BUSSTATE_IDLE;
    while (GEM_I2C_SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};
}

enum gem_i2c_result gem_i2c_write(uint8_t address, uint8_t* data, size_t len) {
    /* Before trying to write, check to see if the bus is busy, if it is,
       bail.
    */
    if (GEM_I2C_SERCOM->I2CM.STATUS.bit.BUSSTATE == BUSSTATE_BUSY) {
        return false;
    }

    /* Address + write flag. */
    GEM_I2C_SERCOM->I2CM.ADDR.bit.ADDR = (address << 0x1ul) | 0;
    /* TODO: Consider a timeout here. */
    while (!GEM_I2C_SERCOM->I2CM.INTFLAG.bit.MB) {};

    /* Check for loss of bus or NACK - in either case we can't continue. */
    if (GEM_I2C_SERCOM->I2CM.STATUS.bit.BUSSTATE != BUSSTATE_OWNER) {
        return GEM_I2C_RESULT_ERR_BUSSTATE;
    }
    if (GEM_I2C_SERCOM->I2CM.STATUS.bit.RXNACK) {
        return GEM_I2C_RESULT_ERR_ADDR_NACK;
    }

    /* Send data bytes. */
    for (size_t i = 0; i < len; i++) {
        /* Send data and wait for TX complete. */
        GEM_I2C_SERCOM->I2CM.DATA.bit.DATA = data[i];

        while (!GEM_I2C_SERCOM->I2CM.INTFLAG.bit.MB) {
            /* Check for loss of arbitration or a bus error. We can't continue if those happen. */
            /* BUSERR is set in addition to ARBLOST if arbitration is lost, so just check that one. */
            if (GEM_I2C_SERCOM->I2CM.STATUS.bit.BUSERR) {
                return GEM_I2C_RESULT_ERR_BUSERR;
            }
        }

        /* If a nack is received we can not continue sending data. */
        if (GEM_I2C_SERCOM->I2CM.STATUS.bit.RXNACK) {
            return GEM_I2C_RESULT_ERR_DATA_NACK;
        }
    }

    /* Send STOP command. */
    GEM_I2C_SERCOM->I2CM.CTRLB.bit.CMD = 3;
    while (GEM_I2C_SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};

    return GEM_I2C_RESULT_SUCCESS;
}