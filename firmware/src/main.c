#include "fix16.h"
#include "gem.h"
#include "printf.h"
#include "sam.h"
#include <stdlib.h>
#include <string.h>

#define FLIP_ADC(value) (4095 - value)
#define ADC_TO_F16(value) (fix16_div(fix16_from_int(value), F16(4095.0)))
#define UINT12_CLAMP(value) value = value > 4095 ? 4095 : value
#define IF_WAGGLED(variable, channel)                                                                                  \
    uint16_t variable = adc_results_live[channel];                                                                     \
    int32_t variable##_delta = variable - adc_results_snapshot[channel];                                               \
    if (labs(variable##_delta) > 50) {                                                                                 \
        variable = FLIP_ADC(variable);
#define IF_WAGGLED_END }

static fix16_t knob_bezier_lut[GEM_KNOB_BEZIER_LUT_LEN];
static struct GemSettings settings;
static uint32_t adc_results_live[GEM_IN_COUNT];
static uint32_t adc_results_snapshot[GEM_IN_COUNT];
static uint32_t* adc_results = adc_results_live;
static struct GemADCErrors knob_errors;
static struct GemButton hard_sync_button = {.port = GEM_HARD_SYNC_BUTTON_PORT, .pin = GEM_HARD_SYNC_BUTTON_PIN};
static bool hard_sync = false;
static struct GemPeriodicWaveform lfo;

struct OscillatorState {
    enum GemADCChannels pitch_cv_channel;
    enum GemADCChannels pitch_knob_channel;
    enum GemADCChannels pulse_width_cv_channel;
    enum GemADCChannels pulse_width_knob_channel;
    struct GemVoiceParams params;
    struct GemSmoothie smooth;
    fix16_t knob_min;
    fix16_t knob_range;
    fix16_t pitch_cv;
    uint16_t pulse_width;
    bool lfo_pwm;
};

static struct OscillatorState castor = {
    .pitch_cv_channel = GEM_IN_CV_A,
    .pitch_knob_channel = GEM_IN_CV_A_POT,
    .pulse_width_cv_channel = GEM_IN_DUTY_A,
    .pulse_width_knob_channel = GEM_IN_DUTY_A_POT,
    .params = {},
    .smooth =
        {
            ._lowpass1 = F16(0),
            ._lowpass2 = F16(0),
        },
};

static struct OscillatorState pollux = {
    .pitch_cv_channel = GEM_IN_CV_B,
    .pitch_knob_channel = GEM_IN_CV_B_POT,
    .pulse_width_cv_channel = GEM_IN_DUTY_B,
    .pulse_width_knob_channel = GEM_IN_DUTY_B_POT,
    .params = {},
    .smooth =
        {
            ._lowpass1 = F16(0),
            ._lowpass2 = F16(0),
        },
};

static void init() {
    /* Tell the world who we are and how we got here. :) */
    printf(gem_build_info_string());

    /* Configure clocks. */
    gem_clocks_init();

    /* Configure systick */
    gem_systick_init();

    /* Initialize NVM */
    gem_nvm_init();

    /* Initialize Random Number Generators */
    gem_random_init(gem_serial_number_low());

    /* Load settings */
    GemSettings_load(&settings);
    GemSettings_print(&settings);

    /* Load the LUT table for DAC codes. */
    gem_load_dac_codes_table();

    /* Intialize the LUT table for the pitch knobs. */
    gem_bezier_1d_2c_generate_lut(
        settings.pitch_knob_nonlinearity,
        fix16_sub(F16(1.0), settings.pitch_knob_nonlinearity),
        knob_bezier_lut,
        GEM_KNOB_BEZIER_LUT_LEN);

    /* Initialize USB. */
    gem_usb_init();

    /* Initialize MIDI interface. */
    gem_register_sysex_commands();

    /* Enable i2c bus for communicating with the DAC. */
    gem_i2c_init();

    /* Enable spi bus, Dotstars, and LED animations. */
    gem_spi_init();
    gem_dotstar_init(settings.led_brightness);
    gem_led_animation_init();

    /* Configure the ADC and channel scanning. */
    gem_adc_init(settings.adc_offset_corr, settings.adc_gain_corr);

    for (size_t i = 0; i < GEM_IN_COUNT; i++) { gem_adc_init_input(&gem_adc_inputs[i]); }
    gem_adc_start_scanning(gem_adc_inputs, GEM_IN_COUNT, adc_results_live);

    /* Configure the timers/PWM generators. */
    gem_pulseout_init();

    /* Configure input for the hard sync button. */
    GemButton_init(&hard_sync_button);

    /* Setup LFO. */
    GemPeriodicWaveform_init(&lfo, gem_triangle, settings.lfo_frequency);

    /* Setup oscillators. */
    knob_errors = (struct GemADCErrors){.offset = settings.knob_offset_corr, .gain = settings.knob_gain_corr};

    castor.knob_min = settings.castor_knob_min;
    castor.knob_range = fix16_sub(settings.castor_knob_max, settings.castor_knob_min);
    castor.smooth.initial_gain = settings.smooth_initial_gain;
    castor.smooth.sensitivity = settings.smooth_sensitivity;
    castor.lfo_pwm = settings.castor_lfo_pwm;

    pollux.knob_min = settings.pollux_knob_min;
    pollux.knob_range = fix16_sub(settings.pollux_knob_max, settings.pollux_knob_min);
    pollux.smooth.initial_gain = settings.smooth_initial_gain;
    pollux.smooth.sensitivity = settings.smooth_sensitivity;
    pollux.lfo_pwm = settings.castor_lfo_pwm;
}

static void calculate_pitch_cv(struct OscillatorState* osc, uint16_t follower_threshold) {
    /*
        The basic pitch CV determination formula is:
        1.0v + (CV in * CV_RANGE) + ((CV knob * KNOB_RANGE) - KNOB_RANGE / 2)
    */

    uint16_t cv_adc_code = FLIP_ADC(adc_results[osc->pitch_cv_channel]);
    fix16_t pitch_cv;

    /*
        This allows the second oscillator to follow the first. If the pitch cv
        in is below a certain threshold, the just use "osc->pitch_cv" (which
        will be set to the first oscillator's pitch cv), otherwise, there's
        enough of an input to calculate the pitch cv from the input.
    */
    if (follower_threshold == 0 || cv_adc_code > follower_threshold) {
        pitch_cv = fix16_add(GEM_CV_BASE_OFFSET, fix16_mul(GEM_CV_INPUT_RANGE, ADC_TO_F16(cv_adc_code)));
    } else {
        pitch_cv = osc->pitch_cv;
    }

    uint16_t knob_adc_code = FLIP_ADC(adc_results[osc->pitch_knob_channel]);
    fix16_t knob_value = fix16_div(gem_adc_correct_errors(fix16_from_int(knob_adc_code), knob_errors), F16(4095.0));
    /* Adjust the knob value using the non-linear lookup table. */
    knob_value = gem_bezier_1d_lut_lookup(knob_bezier_lut, GEM_KNOB_BEZIER_LUT_LEN, knob_value);
    fix16_t pitch_knob = fix16_add(osc->knob_min, fix16_mul(osc->knob_range, knob_value));

    osc->pitch_cv = fix16_add(pitch_cv, pitch_knob);
}

void calculate_pulse_width(struct OscillatorState* osc) {
    uint16_t duty_knob = FLIP_ADC(adc_results[osc->pulse_width_knob_channel]);
    uint16_t duty_cv = FLIP_ADC(adc_results[osc->pulse_width_cv_channel]);

    if (osc->lfo_pwm) {
        fix16_t lfo_multiplier = ADC_TO_F16(duty_cv + duty_knob);
        uint16_t duty_lfo = 2048 + fix16_to_int(fix16_mul(F16(2048), fix16_mul(lfo_multiplier, lfo.value)));
        osc->pulse_width = duty_cv + duty_lfo;
    } else {
        osc->pulse_width = duty_cv + duty_knob;
    }
    UINT12_CLAMP(osc->pulse_width);
}

static void loop() {
    calculate_pitch_cv(&castor, 0);
    pollux.pitch_cv = castor.pitch_cv;
    calculate_pitch_cv(&pollux, settings.pollux_follower_threshold);

    /* Apply smoothing to pitch CVs. */
    castor.pitch_cv = GemSmoothie_step(&castor.smooth, castor.pitch_cv);
    pollux.pitch_cv = GemSmoothie_step(&pollux.smooth, pollux.pitch_cv);

    /*
        Calculate the LFO
    */
    GemPeriodicWaveform_step(&lfo);

    /*
        Calculate chorusing and account for LFO in Pollux's pitch.
    */
    fix16_t chorus_lfo_intensity = ADC_TO_F16(FLIP_ADC(adc_results[GEM_IN_CHORUS_POT]));
    fix16_t chorus_lfo_mod = fix16_mul(settings.chorus_max_intensity, fix16_mul(chorus_lfo_intensity, lfo.value));

    pollux.pitch_cv = fix16_add(pollux.pitch_cv, chorus_lfo_mod);

    /*
        Limit pitch CVs to fit within the parameter table's max value.
    */
    castor.pitch_cv = fix16_clamp(castor.pitch_cv, F16(0), F16(7));
    pollux.pitch_cv = fix16_clamp(pollux.pitch_cv, F16(0), F16(7));

    /*
        Read pulse width inputs and apply LFO if routed.
    */
    calculate_pulse_width(&castor);
    calculate_pulse_width(&pollux);

    /*
        Handle toggling hard sync.
    */

    if (GemButton_tapped(&hard_sync_button)) {
        hard_sync = !hard_sync;
        if (hard_sync) {
            gem_pulseout_hard_sync(true);
            gem_led_animation_set_mode(GEM_LED_MODE_HARD_SYNC);
        } else {
            gem_pulseout_hard_sync(false);
            gem_led_animation_set_mode(GEM_LED_MODE_NORMAL);
        }
    }

    /*
        Calculate the final voice parameters given the input CVs.
    */
    GemVoiceParams_from_cv(
        gem_voice_voltage_and_period_table,
        gem_voice_dac_codes_table,
        gem_voice_param_table_len,
        castor.pitch_cv,
        &castor.params);
    GemVoiceParams_from_cv(
        gem_voice_voltage_and_period_table,
        gem_voice_dac_codes_table,
        gem_voice_param_table_len,
        pollux.pitch_cv,
        &pollux.params);

    /*
        Update timers.
    */

    /* Disable interrupts while changing timers, as any interrupt here could mess them up. */
    __disable_irq();
    gem_pulseout_set_period(0, castor.params.voltage_and_period.period);
    gem_pulseout_set_period(1, pollux.params.voltage_and_period.period);
    __enable_irq();

    /*
        Update DACs.
    */
    gem_mcp_4728_write_channels(
        (struct GemMCP4278Channel){.value = castor.params.dac_codes.castor, .vref = 1},
        (struct GemMCP4278Channel){.value = castor.pulse_width},
        (struct GemMCP4278Channel){.value = pollux.params.dac_codes.pollux, .vref = 1},
        (struct GemMCP4278Channel){.value = pollux.pulse_width});
}

/* This loop is responsible for dealing with the "tweak"
   interface - when the hard sync button is held down the
   interface knobs allow tweaking various settings. */

void tweak_loop() {
    if (GemButton_held(&hard_sync_button)) {
        /* If we just entered tweak mode, copy the adc results to
           the snapshot buffer and point the loop's adc results to
           the snapshot. This prevents the tweak interface for messing
           with the running oscillators. */
        if (GemButton_hold_started(&hard_sync_button)) {
            memcpy(adc_results_snapshot, adc_results_live, GEM_IN_COUNT * sizeof(uint32_t));
            adc_results = adc_results_snapshot;
            gem_led_animation_set_mode(GEM_LED_MODE_TWEAK);
        }

        /* Chorus intensity knob controls lfo frequency in tweak mode. */
        IF_WAGGLED(chorus_pot_code, GEM_IN_CHORUS_POT)
            fix16_t frequency_value = ADC_TO_F16(chorus_pot_code);
            lfo.frequency = fix16_mul(frequency_value, GEM_TWEAK_MAX_LFO_FREQUENCY);
        IF_WAGGLED_END

        /* PWM Knobs control whether or not the LFO gets routed to them. */
        IF_WAGGLED(castor_duty_code, GEM_IN_DUTY_A_POT)
            if (castor_duty_code < 2048) {
                castor.lfo_pwm = false;
            } else {
                castor.lfo_pwm = true;
            }
        IF_WAGGLED_END

        IF_WAGGLED(pollux_duty_code, GEM_IN_DUTY_B_POT)
            if (pollux_duty_code < 2048) {
                pollux.lfo_pwm = false;
            } else {
                pollux.lfo_pwm = true;
            }
        IF_WAGGLED_END

        /* Update LEDs */
        gem_led_tweak_data.lfo_value = lfo.value;
        gem_led_tweak_data.castor_pwm = castor.lfo_pwm;
        gem_led_tweak_data.pollux_pwm = pollux.lfo_pwm;

    } else {
        /* If we just left tweak mode, undo the adc result trickery. */
        if (GemButton_hold_ended(&hard_sync_button)) {
            adc_results = adc_results_live;
            gem_led_animation_set_mode(hard_sync ? GEM_LED_MODE_HARD_SYNC : GEM_LED_MODE_NORMAL);
        }
    }
}

int main(void) {
    init();

    while (1) {
        gem_usb_task();
        gem_midi_task();
        gem_led_animation_step();

        if (gem_adc_results_ready()) {
            GemButton_update(&hard_sync_button);
            loop();
            tweak_loop();
        }
    }

    return 0;
}
