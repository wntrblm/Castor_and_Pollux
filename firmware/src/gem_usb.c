#include "class/midi/midi_device.h"
#include "device/usbd.h"
#include "gem_gpio.h"
#include "sam.h"
#include "tusb.h"

void gem_usb_init() {
    /* Enable the APB clock for USB. */
    PM->APBBMASK.reg |= PM_APBBMASK_USB;
    PM->AHBMASK.reg |= PM_AHBMASK_USB;

    /* Connect GCLK0 (48Mhz) to USB. */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_USB;

    /* Wait until the clock bus is synchronized. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    /* Configure pins for USB. */
    gem_gpio_set_as_output(0, PIN_PA24);
    gem_gpio_set_as_output(0, PIN_PA25);
    gem_gpio_set_mux(0, PIN_PA24, GEM_PMUX_G);
    gem_gpio_set_mux(0, PIN_PA25, GEM_PMUX_G);

    /* Set the priority for the USB interrupt. */
    NVIC_SetPriority(USB_IRQn, 1);

    /* Init tinyusb. */
    tusb_init();
}

void gem_usb_task() { tud_task(); }

bool gem_usb_midi_receive(uint8_t packet[4]) { return tud_midi_receive(packet); }
bool gem_usb_midi_send(uint8_t packet[4]) { return tud_midi_send(packet); }
size_t gem_usb_midi_send_bytes(uint8_t *data, size_t len) { return tud_midi_write(0, data, len); }

void USB_Handler(void) { tud_int_handler(0); }