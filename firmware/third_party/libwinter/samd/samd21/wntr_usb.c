/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "class/midi/midi_device.h"
#include "device/usbd.h"
#include "sam.h"
#include "tusb.h"
#include "wntr_gpio.h"

void wntr_usb_init() {
    /* Enable the APB clock for USB. */
    PM->APBBMASK.reg |= PM_APBBMASK_USB;
    PM->AHBMASK.reg |= PM_AHBMASK_USB;

    /* Connect GCLK0 (48Mhz) to USB. */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_USB;

    /* Wait until the clock bus is synchronized. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    /* Configure pins for USB. */
    wntr_gpio_set_as_output(0, 24);
    wntr_gpio_set_as_output(0, 25);
    wntr_gpio_configure_alt(0, 24, WNTR_PMUX_G);
    wntr_gpio_configure_alt(0, 25, WNTR_PMUX_G);

    /* Set the priority for the USB interrupt. */
    NVIC_SetPriority(USB_IRQn, 1);

    /* Init tinyusb. */
    tusb_init();
}

void wntr_usb_task() { tud_task(); }

void USB_Handler(void) { tud_int_handler(0); }
