#include "sam.h"
#include "gem_gpio.h"
#include "gem_adc.h"
#include "gem_config.h"


static void init_pins() {
    gem_gpio_set_as_input(PIN_BUTTON_PORT, PIN_BUTTON, true);
    gem_gpio_set_as_output(PIN_STATUS_LED_PORT, PIN_STATUS_LED);
    gem_gpio_set_as_output(PIN_STATUS_LED_2_PORT, PIN_STATUS_LED_2);
}

int main(void) {
    // Switch to 8MHz clock (disable prescaler)
    SYSCTRL->OSC8M.bit.PRESC = 0;
    SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE;

    init_pins();

    gem_adc_init();

    gem_adc_init_input(&gem_adc_inputs[0]);
    gem_adc_init_input(&gem_adc_inputs[1]);

    while(1) {
        if(gem_adc_read_sync(&gem_adc_inputs[1]) > 1024) {
            gem_gpio_set(PIN_STATUS_LED_PORT, PIN_STATUS_LED, true);
        } else {
            gem_gpio_set(PIN_STATUS_LED_PORT, PIN_STATUS_LED, false);
        }

        if(gem_adc_read_sync(&gem_adc_inputs[0]) > 1024) {
            gem_gpio_set(PIN_STATUS_LED_2_PORT, PIN_STATUS_LED_2, true);
        } else {
            gem_gpio_set(PIN_STATUS_LED_2_PORT, PIN_STATUS_LED_2, false);
        }
    }

    return 0;
}