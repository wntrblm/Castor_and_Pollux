/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "fix16.h"
#include "gem.h"
#include "printf.h"
#include "sam.h"
#include <stdlib.h>
#include <string.h>

/* Inputs */
static uint32_t adc_results_live_[GEM_IN_COUNT];
static uint32_t adc_results_snapshot_[GEM_IN_COUNT];
static uint32_t* adc_results_ = adc_results_live_;
static struct WntrButton hard_sync_button_ = {.port = GEM_HARD_SYNC_BUTTON_PORT, .pin = GEM_HARD_SYNC_BUTTON_PIN};

/* State */
static struct GemSettings settings_;
static struct {
    wntr_periodic_waveform_function functions[2];
    fix16_t frequencies[2];
    fix16_t factors[2];
    fix16_t phases[2];
} lfo_state_;
static struct WntrMixedPeriodicWaveform lfo_;
static struct GemOscillator castor_;
static struct GemOscillator pollux_;
static bool hard_sync_ = false;

/* Timekeeping */
static uint32_t animation_time_ = 0;
static uint32_t sample_time_ = 0;
static uint32_t idle_cycles_ = 0;

/* Helpers for init_ */
wntr_periodic_waveform_function _lfo_waveshape_setting_to_func(uint8_t n) {
    switch (n) {
        case 0:
            return wntr_triangle;
        case 1:
            return wntr_sine;
        case 2:
            return wntr_sawtooth;
        case 3:
            return wntr_square;
        default:
            return wntr_triangle;
    }
}

/*
    Initializes the core processor, clocks, peripherals, drivers, settings,
    and global state.
*/
static void init_() {
    /*
        Core system initialization.
    */

    /*
        Before doing *anything*, wait for the voltage to stabilize and ensure
        the fuses are set correctly.
    */
    gem_wait_for_stable_voltage();
    wntr_check_bootprot_fuse();

    /* Enable the Micro Trace Buffer for better debug stacktraces. */
    wntr_mtb_init();

    /* Configure clocks & timekeeping. */
    gem_clocks_init();
    wntr_ticks_init();

    /*
        Initialize NVM/Flash storage access - this is needed so that settings
        can be saved to and loaded from NVM.
    */
    gem_nvm_init();

    /* Tell the world who we are and how we got here. :) */
    printf("%s\n", wntr_build_info_string());

    /*
        Peripheral setup
    */

    /* Gemini uses USB MIDI for editing settings and factory configuration. */
    gem_usb_init();

    /* Gemini uses i2c to communicate with the external DAC. */
    gem_i2c_init();
    gem_mcp_4728_init();

    /* Gemini uses SPI to communicate with the Dotstar LEDs. */
    gem_spi_init();

    /*
        Gemini uses the TCC peripheral to output the pulse wave needed to
        drive the oscillators.
    */
    gem_pulseout_init();

    /*
        Global state initialization.
    */

    /*
        Gemini uses a pseudo-random number generator for the LED animation.
        To keep things simple, it just uses its serial number as the seed.
        If it needs to be more fancy in the future it could be changed to read
        the a floating ADC input and use that as the seed.
    */
    uint8_t serial_number[WNTR_SERIAL_NUMBER_LEN];
    wntr_serial_number(serial_number);
    wntr_random_init(*((uint32_t*)(serial_number)));

    /*
        Gemini stores the user's settings in NVM so they have to be explicitly
        loaded.
    */
    GemSettings_load(&settings_);
    GemSettings_print(&settings_);

    /*
        Gemini also stores a "DAC code" table in NVM. This table is used to
        compensate for amplitude loss in the ramp waveform as frequency
        increases.
    */
    gem_load_ramp_table();

    /*
        Driver configuration.
    */

    /* Register SysEx commands used for factory setup. */
    gem_register_sysex_commands();

    /* Enable the Dotstar driver and LED animation. */
    gem_dotstar_init(settings_.led_brightness);
    gem_led_animation_init();

    /*
        Gemini tries to make the most out of SAMD21's ADC.

        First, it uses digital offset and gain error correction which is
        measured during assembly and stored in the user settings. This gives
        Gemini significantly more accurate readings.

        Second, it measures all of the input channels in the background using
        "channel scanning". This frees up the main loop to do other things while
        waiting for new measurements for all the channels.
    */
    gem_adc_init(settings_.adc_offset_corr, settings_.adc_gain_corr);

    for (size_t i = 0; i < GEM_IN_COUNT; i++) { gem_adc_init_input(&gem_adc_inputs[i]); }
    gem_adc_start_scanning(gem_adc_inputs, GEM_IN_COUNT, adc_results_live_);

    /*
        Gemini uses the WntrButton helper for the hard sync button since it
        needs to know when the button is tapped (to toggle hard sync) or
        held (to enter tweak mode).
    */
    WntrButton_init(&hard_sync_button_);

    /*
        Oscillator configuration and initialization.
    */

    /*
        Gemini has an internal low-frequency oscillator that can be used to
        modulate the pitch and pulse width of the oscillators.

        It's current limited to just combining two waveforms, but it could
        support more.
    */
    lfo_state_.functions[0] = _lfo_waveshape_setting_to_func(settings_.lfo_1_waveshape);
    lfo_state_.functions[1] = _lfo_waveshape_setting_to_func(settings_.lfo_2_waveshape);
    lfo_state_.frequencies[0] = settings_.lfo_1_frequency;
    lfo_state_.frequencies[1] = fix16_mul(settings_.lfo_1_frequency, settings_.lfo_2_frequency_ratio);
    lfo_state_.factors[0] = settings_.lfo_1_factor;
    lfo_state_.factors[1] = settings_.lfo_2_factor;
    lfo_state_.phases[0] = F16(0);
    lfo_state_.phases[1] = F16(0);
    WntrMixedPeriodicWaveform_init(
        &lfo_, 2, lfo_state_.functions, lfo_state_.frequencies, lfo_state_.factors, lfo_state_.phases, wntr_ticks());

    /*
        Gemini has two oscillators - Castor & Pollux. For the most part they're
        completely independent: they each have their own pitch and pulse width
        inputs, their own pitch knob range configuration, and their own
        dedicated outputs.

        They share some configuration: the ADC error adjustment, pitch
        knob non-linearity, and smoothie settings.
    */
    gem_oscillator_init(
        (struct WntrErrorCorrection){.offset = settings_.cv_offset_error, .gain = settings_.cv_gain_error},
        settings_.pitch_knob_nonlinearity);

    GemOscillator_init(
        &castor_,
        0,
        GEM_IN_CV_A,
        GEM_IN_CV_A_POT,
        GEM_IN_DUTY_A,
        GEM_IN_DUTY_A_POT,
        settings_.smooth_initial_gain,
        settings_.smooth_sensitivity,
        settings_.base_cv_offset,
        settings_.castor_knob_min,
        settings_.castor_knob_max,
        settings_.castor_lfo_pwm,
        settings_.castor_quantize);

    GemOscillator_init(
        &pollux_,
        1,
        GEM_IN_CV_B,
        GEM_IN_CV_B_POT,
        GEM_IN_DUTY_B,
        GEM_IN_DUTY_B_POT,
        settings_.smooth_initial_gain,
        settings_.smooth_sensitivity,
        settings_.base_cv_offset,
        settings_.pollux_knob_min,
        settings_.pollux_knob_max,
        settings_.pollux_lfo_pwm,
        settings_.pollux_quantize);

    /*
        The oscillators aren't completely independent: Pollux is special.

        First, if Pollux doesn't have any pitch CV input it'll follow Castor's
        pitch.

        Second, Pollux allows the LFO to modulate its pitch. This allows the
        "chorusing" effect when the two oscillators have matching pitches
        and their outputs are mixed.
    */
    pollux_.follower_threshold = settings_.pollux_follower_threshold;
    pollux_.lfo_pitch = true;
}

/*
    This task handles taking in the input state (from the ADC and PORT) and
    updating the oscillators, recalculating their outputs, and applying the
    outputs to the pulse generators and DACs.
*/
static void oscillator_task_() RAMFUNC;

static void oscillator_task_() {
    uint32_t loop_start_time = wntr_ticks();

    /* Toggle hard sync if the button has been tapped. */
    WntrButton_update(&hard_sync_button_);

    if (WntrButton_tapped(&hard_sync_button_)) {
        hard_sync_ = !hard_sync_;
        if (hard_sync_) {
            gem_pulseout_hard_sync(true);
            gem_led_animation_set_mode(GEM_LED_MODE_HARD_SYNC);
        } else {
            gem_pulseout_hard_sync(false);
            gem_led_animation_set_mode(GEM_LED_MODE_NORMAL);
        }
    }

    /*
        Update the internal LFO used for modulating pitch and pulse width.
    */
    fix16_t lfo_value = WntrMixedPeriodicWaveform_step(&lfo_, loop_start_time);
    gem_led_tweak_data.lfo_value = lfo_value;
    fix16_t pitch_lfo_intensity = UINT12_NORMALIZE(UINT12_INVERT(adc_results_[GEM_IN_CHORUS_POT]));
    fix16_t pitch_lfo_value = fix16_mul(settings_.chorus_max_intensity, fix16_mul(pitch_lfo_intensity, lfo_value));

    /*
        Update both oscillator's internal state based on the ADC inputs.
    */
    struct GemOscillatorInputs inputs = {
        .adc = adc_results_, .lfo_pulse_width = lfo_value, .lfo_pitch = pitch_lfo_value};

    GemOscillator_update(&castor_, inputs);

    /*
        Make Pollux follow Castor's pitch if there's nothing connected to
        Pollux's pitch CV input.

        If GemOscillator_update detects no input it'll use the value set here.
    */
    pollux_.pitch = castor_.pitch;
    GemOscillator_update(&pollux_, inputs);

    /*
        Oscillator post-update applies pitch smoothing and limiting and
        finalizes the outputs.
    */
    GemOscillator_post_update(&castor_, inputs);
    GemOscillator_post_update(&pollux_, inputs);

    /*
        Update the timers with their new values calculated from their
        oscillator's pitch.

        It's important that these get updated at essentially the same time so
        that they have a stable phase relationship. Therefore, interrupts are
        disabled while Gemini modifies the timer configuration.
    */
    __disable_irq();
    gem_pulseout_set_period(0, castor_.outputs.period);
    gem_pulseout_set_period(1, pollux_.outputs.period);
    __enable_irq();

    /*
        Update the DAC outputs with the new charge and pulse width
        values.

        Each oscillator requires two DAC outputs.

        The first one is used to compensation for amplitude loss as frequency
        increases. Higher voltage allows the ramp core's integrating capacitor
        to charge more quickly and reach a higher voltage before the timer
        resets the ramp.

        The second is used to control the pulse-width of the pulse waveform.
        The output voltage goes into a comparator that compares against the
        ramp waveform to generate a pulse.
    */
    gem_mcp_4728_write_channels(
        (struct GemMCP4278Channel){.value = castor_.outputs.ramp_cv},
        (struct GemMCP4278Channel){.value = castor_.pulse_width},
        (struct GemMCP4278Channel){.value = pollux_.outputs.ramp_cv},
        (struct GemMCP4278Channel){.value = pollux_.pulse_width});

    /*
        Update the loop timer.
    */
    uint16_t loop_time = (uint16_t)(wntr_ticks() - loop_start_time);

    /*
        To help with testing and debugging, Gemini can send its state over
        MIDI SysEx to the monitoring script in `/factory/monitor.py`.
    */
    struct GemMonitorUpdate monitor_update = {
        .castor_pitch_knob = castor_.pitch_knob,
        .castor_pitch_cv = castor_.pitch_cv,
        .castor_pulse_width_knob = castor_.pulse_width_knob,
        .castor_pulse_width_cv = castor_.pulse_width_cv,
        .pollux_pitch_knob = pollux_.pitch_knob,
        .pollux_pitch_cv = pollux_.pitch_cv,
        .pollux_pulse_width_knob = pollux_.pulse_width_knob,
        .pollux_pulse_width_cv = pollux_.pulse_width_cv,
        .button_state = hard_sync_button_.state,
        .lfo_intensity = pitch_lfo_intensity,
        .loop_time = loop_time,
        .animation_time = (uint16_t)(animation_time_),
        .sample_time = (uint16_t)(idle_cycles_)};

    gem_sysex_send_monitor_update(&monitor_update);
}

/*
    This task is responsible for dealing with the "tweak" interface: when the
    hard sync button is held down the user can customize module settings using
    the input knobs.
*/

#define IF_WAGGLED(variable, channel)                                                                                  \
    uint16_t variable = adc_results_live_[channel];                                                                    \
    int32_t variable##_delta = variable - adc_results_snapshot_[channel];                                              \
    if (labs(variable##_delta) > 50) {                                                                                 \
        variable = UINT12_INVERT(variable);
#define IF_WAGGLED_END }

static void tweak_task_() {
    if (WntrButton_held(&hard_sync_button_)) {
        /*
            Tweak mode takes over the interface, so don't let the
            oscillator_step task see changes to the ADC inputs while we're
            in tweak mode.

            This is done by taking a "snapshot" of the current ADC readings and
            pointing oscillator_step's `adc_results` to the snapshot. When
            exiting tweak mode the `adc_results` pointer is set back to the
            live ADC results.
        */
        if (WntrButton_hold_started(&hard_sync_button_)) {
            memcpy(adc_results_snapshot_, adc_results_live_, GEM_IN_COUNT * sizeof(uint32_t));
            adc_results_ = adc_results_snapshot_;
            gem_led_animation_set_mode(GEM_LED_MODE_TWEAK);
        }

        /* Chorus intensity knob controls the LFO frequency in tweak mode. */
        IF_WAGGLED(chorus_pot_code, GEM_IN_CHORUS_POT)
            fix16_t frequency_value = UINT12_NORMALIZE(chorus_pot_code);
            lfo_state_.frequencies[0] = fix16_mul(frequency_value, GEM_TWEAK_MAX_LFO_FREQUENCY);
        IF_WAGGLED_END

        /* PWM Knobs control whether or not the LFO gets routed to them. */
        IF_WAGGLED(castor_duty_code, GEM_IN_DUTY_A_POT)
            if (castor_duty_code < 2048) {
                castor_.lfo_pwm = false;
            } else {
                castor_.lfo_pwm = true;
            }
        IF_WAGGLED_END

        IF_WAGGLED(pollux_duty_code, GEM_IN_DUTY_B_POT)
            if (pollux_duty_code < 2048) {
                pollux_.lfo_pwm = false;
            } else {
                pollux_.lfo_pwm = true;
            }
        IF_WAGGLED_END

        /*
            Tweak mode uses the LEDs to tell the user what the settings are.
        */
        gem_led_tweak_data.castor_pwm = castor_.lfo_pwm;
        gem_led_tweak_data.pollux_pwm = pollux_.lfo_pwm;

    } else {
        /* If we just left tweak mode, undo the ADC result trickery. */
        if (WntrButton_hold_ended(&hard_sync_button_)) {
            adc_results_ = adc_results_live_;
            gem_led_animation_set_mode(hard_sync_ ? GEM_LED_MODE_HARD_SYNC : GEM_LED_MODE_NORMAL);
        }
    }
}

/* Gemini uses MIDI for factory setup and for the settings editor. */
static void midi_task_() {
    struct WntrMIDIMessage msg = {};
    if (!wntr_midi_receive(&msg)) {
        return;
    }
    if (msg.code_index == MIDI_CODE_INDEX_SYSEX_START_OR_CONTINUE) {
        wntr_midi_dispatch_sysex();
    }
}

int main(void) {
    init_();

    uint32_t last_sample_time = wntr_ticks();

    while (1) {
        /*
            Gemini doesn't use an RTOS, instead, it just runs a few tasks that
            are expected to be behave and yield time to other tasks.
        */
        gem_usb_task();
        midi_task_();

        /*
            The LED animation task internally ensures that it only runs once
            every few milliseconds. See GEM_ANIMATION_INTERVAL.
        */
        uint32_t animation_start_time = wntr_ticks();
        if (gem_led_animation_step()) {
            animation_time_ = wntr_ticks() - animation_start_time;
        }

        /*
            The oscillator & tweak tasks only need to be called when there's
            a new set of ADC readings ready. The ADC is constantly scanning
            in the background, so that gives the USB, MIDI, and LED animation
            tasks time to run between oscillator updates.
        */
        if (gem_adc_results_ready()) {
            sample_time_ = (uint16_t)(wntr_ticks() - last_sample_time);
            last_sample_time = wntr_ticks();
            oscillator_task_();
            tweak_task_();
            idle_cycles_ = 0;
        } else {
            idle_cycles_++;
        }
    }

    return 0;
}
