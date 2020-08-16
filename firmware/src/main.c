#include <stdio.h>
#include "sam.h"
#include "gem_clocks.h"
#include "gem_gpio.h"
#include "gem_adc.h"
#include "gem_pulseout.h"
#include "gem_i2c.h"
#include "gem_config.h"


static void init_pins() {
    gem_gpio_set_as_input(PIN_BUTTON_PORT, PIN_BUTTON, true);
    // gem_gpio_set_as_output(PIN_STATUS_LED_PORT, PIN_STATUS_LED);
    // gem_gpio_set_as_output(PIN_STATUS_LED_2_PORT, PIN_STATUS_LED_2);
}

static uint32_t adc_results[2];

int main(void) {
    /* Configure clocks. */
    gem_clocks_init();

    // Initialize any configuration data and functionality,
    // such as printf() in debug mode.
    gem_config_init();

    init_pins();

    gem_adc_init();
    gem_adc_init_input(&gem_adc_inputs[0]);
    gem_adc_init_input(&gem_adc_inputs[1]);
    gem_adc_start_scanning(gem_adc_inputs, 2, adc_results);


    gem_pulseout_init();
    gem_pulseout_set_frequency(0, 2^24);
    gem_pulseout_set_duty(0, 0);
    gem_pulseout_set_frequency(1, 2^24);
    gem_pulseout_set_duty(1, 0);

    gem_i2c_init();

    uint8_t i2c_address = 0x60;
    while(1) {
        if(gem_adc_results_ready()) {
            printf("Ch1: %lu, Ch2: %lu\r\n", adc_results[0], adc_results[1]);

            gem_pulseout_set_duty(0, adc_results[0] / 4096.0f);
            gem_pulseout_set_duty(1, adc_results[1] / 4096.0f);


            uint8_t i2c_data[] = {
                0b01011010,
                (adc_results[1] >> 8),
                (adc_results[1] & 0xFF),
            };

            gem_i2c_write(i2c_address, i2c_data, 3);
        }
    }

    return 0;
}