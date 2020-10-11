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
#include "gem_midi_core.h"
#include "gem_midi_sysex.h"
#include "gem_nvm.h"
#include "gem_pulseout.h"
#include "gem_quant.h"
#include "gem_random.h"
#include "gem_serial_number.h"
#include "gem_smoothie.h"
#include "gem_spi.h"
#include "gem_systick.h"
#include "gem_usb.h"
#include "gem_voice_param_table.h"
#include "gem_voice_params.h"
#include "gem_waveforms.h"
#include "printf.h"
#include "sam.h"

static struct gem_nvm_settings settings;
static uint32_t adc_results[GEM_IN_COUNT];
static uint32_t last_update = 0;
static uint32_t last_sync_button_change = 0;
static bool hard_sync_reading = false;
static bool previous_hard_sync_reading = false;
static bool hard_sync = false;
static struct gem_voice_params castor_params = {};
static struct gem_voice_params pollux_params = {};
static fix16_t chorus_lfo_phase = 0;
static fix16_t castor_knob_range;
static fix16_t pollux_knob_range;
static struct gem_smoothie_state castor_smooth = {
    .initial_gain = F16(0.3),
    .sensitivity = F16(13.0),
    ._lowpass1 = F16(0),
    ._lowpass2 = F16(0),
};
static struct gem_smoothie_state pollux_smooth = {
    .initial_gain = F16(0.3),
    .sensitivity = F16(13.0),
    ._lowpass1 = F16(0),
    ._lowpass2 = F16(0),
};

void process_sysex_event(enum gem_sysex_event event);

int main(void) {
    /* Configure clocks. */
    gem_clocks_init();

    /* Configure systick */
    gem_systick_init();

    /* Initialize NVM */
    gem_nvm_init();

    /* Initialize Random Number Generators */
    gem_random_init(gem_serial_number_low());

    /* Initialize any configuration data and functionality, such as printf() in debug mode. */
    gem_config_init();

    /* Load settings */
    bool valid_settings = gem_config_get_nvm_settings(&settings);

    if (!valid_settings) {
        printf("Failed to load settings.\r\n");
    } else {
        printf("Loaded settings.\r\n");
        printf(
            "Settings:\r\n ADC gain: %u\r\n ADC offset: %i code points\r\n LED brightness: %u / 255\r\n",
            settings.adc_gain_corr,
            (int16_t)(settings.adc_offset_corr),
            settings.led_brightness);
        char fix16buf[13];
        fix16_to_str(settings.castor_knob_min, fix16buf, 2);
        printf(" Castor knob min: %s v/oct\r\n", fix16buf);
        fix16_to_str(settings.castor_knob_max, fix16buf, 2);
        printf(" Castor knob max: %s v/oct\r\n", fix16buf);
        fix16_to_str(settings.pollux_knob_min, fix16buf, 2);
        printf(" Pollux knob max: %s v/oct\r\n", fix16buf);
        fix16_to_str(settings.pollux_knob_max, fix16buf, 2);
        printf(" Pollux knob max: %s v/oct\r\n", fix16buf);
        fix16_to_str(settings.chorus_frequency, fix16buf, 2);
        printf(" Chorus frequency: %s Hz\r\n", fix16buf);
        fix16_to_str(settings.chorus_max_intensity, fix16buf, 2);
        printf(" Chorus intensity: %s v/oct\r\n", fix16buf);
    }

    castor_knob_range = fix16_sub(settings.pollux_knob_max, settings.pollux_knob_min);
    pollux_knob_range = fix16_sub(settings.pollux_knob_max, settings.pollux_knob_min);

    /* Initialize USB. */
    gem_usb_init();

    /* Initialize MIDI interface. */
    gem_midi_set_sysex_callback(gem_process_sysex);
    gem_set_sysex_event_callback(process_sysex_event);

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

    /* Configure input for the hard sync button. */
    gem_gpio_set_as_input(GEM_HARD_SYNC_BUTTON_PORT, GEM_HARD_SYNC_BUTTON_PIN, true);

    while (1) {
        gem_usb_task();
        gem_midi_task();
        gem_led_animation_step();

        if (gem_adc_results_ready()) {
            /* Castor's pitch determination is

                1.0v + (CV in * 6.0v) + ((CV knob * 2.0) - 1.0)

            */
            // TODO: Add back quantizations?
            uint16_t castor_pitch_cv_code = (4095 - adc_results[GEM_IN_CV_A]);
            fix16_t castor_pitch_cv_value = fix16_div(fix16_from_int(castor_pitch_cv_code), F16(4095.0));

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

                    1.0v + (Castor CV * 6.0v) + ((CV knob * 2.0) - 1.0)

                Else it uses the input CV:

                    1.0v + (CV in * 6.0v) + ((CV knob * 2.0) - 1.0)

                This means that if there's no pitch input, then Pollux is the same pitch as
                Castor but fine-tuned up or down using the CV knob. If there is a pitch CV
                applied, the the knob just acts as a normal fine-tune.
            */
            fix16_t pollux_pitch_cv = castor_pitch_cv;

            uint16_t pollux_pitch_cv_code = (4095 - adc_results[GEM_IN_CV_B]);

            // TODO: Maybe adjust this threshold.
            if (pollux_pitch_cv_code > 6) {
                fix16_t pollux_pitch_cv_value = fix16_div(fix16_from_int(pollux_pitch_cv_code), F16(4095.0));
                pollux_pitch_cv = fix16_add(GEM_CV_BASE_OFFSET, fix16_mul(GEM_CV_INPUT_RANGE, pollux_pitch_cv_value));
            }

            uint16_t pollux_pitch_knob_code = (4095 - adc_results[GEM_IN_CV_B_POT]);
            fix16_t pollux_pitch_knob_value = fix16_div(fix16_from_int(pollux_pitch_knob_code), F16(4095.0));
            fix16_t pollux_pitch_knob =
                fix16_add(settings.pollux_knob_min, fix16_mul(pollux_knob_range, pollux_pitch_knob_value));

            pollux_pitch_cv = fix16_add(pollux_pitch_cv, pollux_pitch_knob);

            /* Apply smoothing to input CVs. */
            castor_pitch_cv = gem_smoothie_step(&castor_smooth, castor_pitch_cv);
            pollux_pitch_cv = gem_smoothie_step(&pollux_smooth, pollux_pitch_cv);

            /* Test - dump pitch cv to midi. */
            // gem_usb_midi_send((uint8_t[4]){0x04, 0xF0, 0x77, 0xA});
            // gem_usb_midi_send((uint8_t[4]){
            //     0x04, (castor_pitch_cv >> 28) & 0xF, (castor_pitch_cv >> 24) & 0xF, (castor_pitch_cv >> 20) & 0xF});
            // gem_usb_midi_send((uint8_t[4]){
            //     0x04, (castor_pitch_cv >> 16) & 0xF, (castor_pitch_cv >> 12) & 0xF, (castor_pitch_cv >> 8) & 0xF});
            // gem_usb_midi_send((uint8_t[4]){0x07, (castor_pitch_cv >> 4) & 0xF, (castor_pitch_cv)&0xF, 0xF7});

            /*
                Calculate the chorus LFO and account for LFO in Pollux's pitch.
            */
            uint32_t now = gem_get_ticks();
            uint32_t delta = now - last_update;

            chorus_lfo_phase += fix16_mul(fix16_div(settings.chorus_frequency, F16(1000.0)), fix16_from_int(delta));
            if (chorus_lfo_phase > F16(1.0))
                chorus_lfo_phase = fix16_sub(chorus_lfo_phase, F16(1.0));

            uint16_t chorus_lfo_amount_code = (4095 - adc_results[GEM_IN_CHORUS_POT]);
            fix16_t chorus_lfo_amount = fix16_div(fix16_from_int(chorus_lfo_amount_code), F16(4095.0));

            fix16_t chorus_lfo_mod =
                fix16_mul(settings.chorus_max_intensity, fix16_mul(chorus_lfo_amount, gem_triangle(chorus_lfo_phase)));
            pollux_pitch_cv = fix16_add(pollux_pitch_cv, chorus_lfo_mod);

            /*
                Limit pitch CVs to fit within the parameter table's max value.
            */
            if (castor_pitch_cv < F16(0.0))
                castor_pitch_cv = F16(0.0);
            if (pollux_pitch_cv < F16(0.0))
                pollux_pitch_cv = F16(0.0);
            if (castor_pitch_cv > F16(7.0))
                castor_pitch_cv = F16(7.0);
            if (pollux_pitch_cv > F16(7.0))
                pollux_pitch_cv = F16(7.0);

            /*
                PWM inputs.
            */

            uint16_t castor_duty = 4095 - adc_results[GEM_IN_DUTY_A_POT];
            castor_duty += 4095 - adc_results[GEM_IN_DUTY_A];
            if (castor_duty > 4095)
                castor_duty = 4095;
            uint16_t pollux_duty = 4095 - adc_results[GEM_IN_DUTY_B_POT];
            pollux_duty += 4095 - adc_results[GEM_IN_DUTY_B];
            if (pollux_duty > 4095)
                pollux_duty = 4095;

            /*
                Check for hard sync.
            */
            hard_sync_reading = gem_gpio_get(GEM_HARD_SYNC_BUTTON_PORT, GEM_HARD_SYNC_BUTTON_PIN);
            if (now - last_sync_button_change > GEM_HARD_SYNC_BUTTON_DEBOUNCE && hard_sync_reading == false &&
                previous_hard_sync_reading == true) {
                last_sync_button_change = now;
                hard_sync = !hard_sync;
                if (hard_sync) {
                    gem_pulseout_hard_sync(true);
                    gem_led_animation_set_mode(GEM_LED_MODE_HARD_SYNC);
                } else {
                    gem_pulseout_hard_sync(false);
                    gem_led_animation_set_mode(GEM_LED_MODE_NORMAL);
                }
            }
            previous_hard_sync_reading = hard_sync_reading;

            /*
                Calculate the final voice parameters given the input CVs.
            */
            gem_voice_params_from_cv(gem_voice_param_table, gem_voice_param_table_len, castor_pitch_cv, &castor_params);
            gem_voice_params_from_cv(gem_voice_param_table, gem_voice_param_table_len, pollux_pitch_cv, &pollux_params);

            /*
                Update timers.
            */

            /* Disable interrupts while changing timers, as any interrupt here could mess them up. */
            __disable_irq();
            gem_pulseout_set_period(0, castor_params.period_reg);
            gem_pulseout_set_period(1, pollux_params.period_reg);
            __enable_irq();

            /*
                Update DACs.
            */
            gem_mcp_4728_write_channels(
                (struct gem_mcp4728_channel){.value = castor_params.castor_dac_code, .vref = 1},
                (struct gem_mcp4728_channel){.value = castor_duty},
                (struct gem_mcp4728_channel){.value = pollux_params.pollux_dac_code, .vref = 1},
                (struct gem_mcp4728_channel){.value = pollux_duty});

            last_update = gem_get_ticks();
        }
    }

    return 0;
}

void process_sysex_event(enum gem_sysex_event event) {
    switch (event) {
        case GEM_SYSEX_EVENT_CALIBRATION_MODE:
            /* For calibration mode, stop scanning ADC channels. This will also stop
                the main loop above from continuing to change the outputs. */
            gem_adc_stop_scanning();
            gem_led_animation_set_mode(GEM_LED_MODE_CALIBRATION);
            break;

        default:
            break;
    }
}