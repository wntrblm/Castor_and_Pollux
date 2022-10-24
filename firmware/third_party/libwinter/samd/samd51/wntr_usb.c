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

#define USB_GCLK GCLK_PCHCTRL_GEN_GCLK1

void wntr_usb_init() {
    /* Setup clocks */
    GCLK->PCHCTRL[USB_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | USB_GCLK;
    while (!GCLK->PCHCTRL[USB_GCLK_ID].bit.CHEN) {};
    MCLK->AHBMASK.reg |= MCLK_AHBMASK_USB;
    MCLK->APBBMASK.reg |= MCLK_APBBMASK_USB;

    /* Configure pins for USB. */
    wntr_gpio_set_as_output(0, 24);
    wntr_gpio_configure_alt(0, 24, WNTR_PMUX_H);
    wntr_gpio_set_as_output(0, 25);
    wntr_gpio_configure_alt(0, 25, WNTR_PMUX_H);

    /* Enable USB interrupts */
    NVIC_SetPriority(USB_0_IRQn, 1);
    NVIC_SetPriority(USB_1_IRQn, 1);
    NVIC_SetPriority(USB_2_IRQn, 1);
    NVIC_SetPriority(USB_3_IRQn, 1);

    /* Init tinyusb. */
    tusb_init();
}

void wntr_usb_task() { tud_task(); }

void USB_0_Handler(void) { tud_int_handler(0); }
void USB_1_Handler(void) { tud_int_handler(0); }
void USB_2_Handler(void) { tud_int_handler(0); }
void USB_3_Handler(void) { tud_int_handler(0); }
