#include "fix16.h"
#include "gem_adc.h"
#include "gem_clocks.h"
#include "gem_colorspace.h"
#include "gem_config.h"
#include "gem_dotstar.h"
#include "gem_gpio.h"
#include "gem_i2c.h"
#include "gem_led_animation.h"
#include "gem_mcp4728.h"
#include "gem_midi.h"
#include "gem_nvm.h"
#include "gem_pulseout.h"
#include "gem_quant.h"
#include "gem_spi.h"
#include "gem_systick.h"
#include "gem_usb.h"
#include "gem_voice_param_table.h"
#include "gem_voice_params.h"
#include "gem_waveforms.h"
#include "sam.h"
#include <stdio.h>

static struct gem_nvm_settings settings;
static uint32_t adc_results[GEM_IN_COUNT];
static uint32_t last_update = 0;
static struct gem_voice_params castor_params = {};
static struct gem_voice_params pollux_params = {};
static fix16_t chorus_lfo_phase = 0;
static fix16_t castor_knob_range;
static fix16_t pollux_knob_range;

void midi_event_callback(enum gem_midi_event event);

int main(void) {
    /* Configure clocks. */
    gem_clocks_init();

    /* Configure systick */
    gem_systick_init();

    /* Initialize NVM */
    gem_nvm_init();

    // Initialize any configuration data and functionality,
    // such as printf() in debug mode.
    gem_config_init();

    /* Load settings */
    bool valid_settings = gem_config_get_nvm_settings(&settings);

    if (!valid_settings) {
        __wrap_printf("Failed to load settings.\r\n");
    } else {
        __wrap_printf("Loaded settings.\r\n");
        printf(
            "Settings: 0x%x, 0x%x, 0x%x\r\n",
            settings.adc_gain_corr,
            settings.adc_offset_corr,
            settings.led_brightness);
    }

    castor_knob_range = fix16_sub(settings.pollux_knob_max, settings.pollux_knob_min);
    pollux_knob_range = fix16_sub(settings.pollux_knob_max, settings.pollux_knob_min);

    /* Initialize USB. */
    gem_usb_init();

    /* Initialize MIDI interface. */
    gem_midi_set_event_callback(midi_event_callback);

    /* Enable i2c bus for communicating with the DAC. */
    gem_i2c_init();

    /* Enable spi bus, Dotstars, and LED animations. */
    gem_spi_init();
    gem_dotstar_init(settings.led_brightness);
    gem_led_animation_init();

    /* Configure the ADC and channel scanning. */
    gem_adc_init(settings.adc_offset_corr, settings.adc_gain_corr);

    for (size_t i = 0; i < GEM_IN_COUNT; i++) { gem_adc_init_input(&gem_adc_inputs[i]); }
    gem_adc_start_scanning(gem_adc_inputs, GEM_IN_COUNT, adc_results);

    /* Configure the timers/PWM generators. */
    gem_pulseout_init();

    while (1) {
        gem_usb_task();
        gem_midi_task();

        /* TODO: Only run this every couple of ms, maybe once every 16? */
        gem_led_animation_step();

        if (gem_adc_results_ready()) {
            uint32_t now = gem_get_ticks();
            uint32_t delta = now - last_update;

            /* Castor's pitch determination is

                1.0v + quant(CV in) + qaunt(CV knob * 6.0)

                This means that Castor gets a full range out of
                its pitch input and pitch knob.
            */
            // TODO: Add back quantizations.
            uint16_t castor_pitch_cv_code = (4095 - adc_results[GEM_IN_CV_A]);
            fix16_t castor_pitch_cv_value = fix16_div(fix16_from_int(castor_pitch_cv_code), F16(4095.0));
            ;
            fix16_t castor_pitch_cv =
                fix16_add(GEM_CV_BASE_OFFSET, fix16_mul(GEM_CV_INPUT_RANGE, castor_pitch_cv_value));

            uint16_t castor_pitch_knob_code = (4095 - adc_results[GEM_IN_CV_A_POT]);
            fix16_t castor_pitch_knob_value = fix16_div(fix16_from_int(castor_pitch_knob_code), F16(4095.0));
            fix16_t castor_pitch_knob =
                fix16_add(settings.castor_knob_min, fix16_mul(castor_knob_range, castor_pitch_knob_value));

            castor_pitch_cv = fix16_add(castor_pitch_cv, castor_pitch_knob);

            /* Pollux is the "follower", so its pitch determination is based on whether or not
                it has input CV.

                If CV in == 0, then it follows Castor:

                    CV = Castor CV + -1.0v + (2.0v * CV knob)

                Else it uses the input CV:

                    CV = 1.0v + quant(CV in) + -1.0v + (2.0v * CV knob)

                This means that if there's no pitch input, then Pollux is the same pitch as
                Castor but fine-tuned up or down using the CV knob. If there is a pitch CV
                applied, the the knob just acts as a normal fine-tune.
            */
            fix16_t pollux_pitch_cv = castor_pitch_cv;

            uint16_t pollux_pitch_cv_code = (4095 - adc_results[GEM_IN_CV_B]);

            // TODO: Maybe adjust this threshold.
            if (pollux_pitch_cv_code > 6) {
                fix16_t pollux_pitch_cv_value = fix16_div(fix16_from_int(pollux_pitch_cv_code), F16(4095.0));
                ;
                pollux_pitch_cv = fix16_add(GEM_CV_BASE_OFFSET, fix16_mul(GEM_CV_INPUT_RANGE, pollux_pitch_cv_value));
            }

            uint16_t pollux_pitch_knob_code = (4095 - adc_results[GEM_IN_CV_B_POT]);
            fix16_t pollux_pitch_knob_value = fix16_div(fix16_from_int(pollux_pitch_knob_code), F16(4095.0));
            fix16_t pollux_pitch_knob =
                fix16_add(settings.pollux_knob_min, fix16_mul(pollux_knob_range, pollux_pitch_knob_value));

            pollux_pitch_cv = fix16_add(pollux_pitch_cv, pollux_pitch_knob);

            /* Calculate the chorus LFO and account for LFO in Pollux's pitch. */
            chorus_lfo_phase +=
                fix16_mul(fix16_div(F16(GEM_CHORUS_LFO_FREQUENCY), F16(1000.0f)), fix16_from_int(delta));
            if (chorus_lfo_phase > F16(1.0f))
                chorus_lfo_phase = fix16_sub(chorus_lfo_phase, F16(1.0f));

            uint16_t chorus_lfo_amount_code = (4095 - adc_results[GEM_IN_CHORUS_POT]);
            fix16_t chorus_lfo_amount = fix16_div(fix16_from_int(chorus_lfo_amount_code), F16(4095.0f));

            fix16_t chorus_lfo_mod =
                fix16_mul(F16(0.05f), fix16_mul(chorus_lfo_amount, gem_triangle(chorus_lfo_phase)));
            pollux_pitch_cv = fix16_add(pollux_pitch_cv, chorus_lfo_mod);

            // /* Limit pitch CVs to fit within the parameter table's max value. */
            if (castor_pitch_cv > F16(7.0f))
                castor_pitch_cv = F16(7.0f);
            if (pollux_pitch_cv > F16(7.0f))
                pollux_pitch_cv = F16(7.0f);

            /* TODO: maybe adjust these ranges once tested with new pots. */
            uint16_t castor_duty = 4095 - adc_results[GEM_IN_DUTY_A_POT];
            uint16_t pollux_duty = 4095 - adc_results[GEM_IN_DUTY_B_POT];

            gem_voice_params_from_cv(gem_voice_param_table, gem_voice_param_table_len, castor_pitch_cv, &castor_params);
            gem_voice_params_from_cv(gem_voice_param_table, gem_voice_param_table_len, pollux_pitch_cv, &pollux_params);

            /* Disable interrupts while changing timers, as any interrupt here could mess them up. */
            __disable_irq();
            gem_pulseout_set_period(0, castor_params.period_reg);
            gem_pulseout_set_period(1, pollux_params.period_reg);
            __enable_irq();

            // TODO: Enable this once a button is wired up.
            // if(!gem_gpio_get(GEM_IN_SYNC_PORT, GEM_IN_SYNC_PIN)) {
            //     gem_pulseout_hard_sync(true);
            // } else {
            //     gem_pulseout_hard_sync(false);
            // }

            gem_mcp_4728_write_channels(
                (struct gem_mcp4728_channel){.value = castor_params.castor_dac_code, .vref = 1},
                (struct gem_mcp4728_channel){.value = castor_duty},
                (struct gem_mcp4728_channel){.value = pollux_params.pollux_dac_code, .vref = 1},
                (struct gem_mcp4728_channel){.value = pollux_duty});

            last_update = gem_get_ticks();
            uint32_t loop_time = last_update - now;
            printf("loop time: %lu\r\n", loop_time);
        }
    }

    return 0;
}

void midi_event_callback(enum gem_midi_event event) {
    switch (event) {
        case GEM_MIDI_EVENT_CALIBRATION_MODE:
            /* For calibration mode, stop scanning ADC channels. This will also stop
                the main loop above from continuing to change the outputs. */
            gem_adc_stop_scanning();
            gem_led_animation_set_mode(GEM_LED_MODE_CALIBRATION);
            break;

        default:
            break;
    }
}