/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_led_animation.h"
#include "fix16.h"
#include "gem_config.h"
#include "gem_dotstar.h"
#include "wntr_colorspace.h"
#include "wntr_random.h"
#include "wntr_ticks.h"
#include "wntr_waveforms.h"
#include <math.h>
#include <stdlib.h>

/* Macros */

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a < b ? b : a)
#define MINMAX(x, lo, hi) (MIN(hi, MAX(lo, x)))

/* Static variables */

struct GemLEDInputs gem_led_inputs = {
    .tweaking = false,
    .lfo_amplitude = F16(0),
    .lfo_gain = F16(0),
    .lfo_mod_a = 0,
    .lfo_mod_b = 0,
};

static struct GemLEDCfg cfg_;

static enum GemMode mode_ = GEM_MODE_NORMAL;
static uint32_t last_update_;
static fix16_t phase_a_ = F16(0);
static uint32_t hue_accum_;
static uint8_t sparkles_[GEM_MAX_DOTSTAR_COUNT];
static bool transitioning_ = false;

/* Forward declarations. */

static fix16_t noise(fix16_t) RAMFUNC;
static void animation_step_transition_(const struct GemDotstarCfg* dotstar, uint32_t delta) RAMFUNC;
static void animation_step_sparkles_(const struct GemDotstarCfg* dotstar, uint32_t delta) RAMFUNC;
static void animation_step_normal_(const struct GemDotstarCfg* dotstar, uint32_t delta) RAMFUNC;
static void animation_step_calibration_(const struct GemDotstarCfg* dotstar, uint32_t ticks);
static void animation_step_tweak_(const struct GemDotstarCfg* dotstar, uint32_t ticks) RAMFUNC;

/* Public functions. */

void gem_led_animation_init(const struct GemLEDCfg cfg) {
    cfg_ = cfg;
    last_update_ = wntr_ticks();
}

void gem_led_animation_set_mode(enum GemMode mode) {
    mode_ = mode;
    phase_a_ = F16(0);
    transitioning_ = true;
    switch (mode_) {
        case GEM_MODE_NORMAL:
            hue_accum_ = 13107;
            break;
        case GEM_MODE_LFO_FM:
            hue_accum_ = 21845;
            break;
        case GEM_MODE_LFO_PWM:
            hue_accum_ = 39321;
            break;
        case GEM_MODE_HARD_SYNC:
            hue_accum_ = 52428;
            break;
        default:
            break;
    }
}

bool gem_led_animation_step(const struct GemDotstarCfg* dotstar) {
    uint32_t ticks = wntr_ticks();
    uint32_t delta = ticks - last_update_;
    if (delta < GEM_ANIMATION_INTERVAL) {
        return false;
    }

    last_update_ = ticks;

    if (transitioning_) {
        animation_step_transition_(dotstar, delta);
    } else if (gem_led_inputs.tweaking) {
        animation_step_tweak_(dotstar, delta);
    } else {
        switch (mode_) {
            case GEM_MODE_NORMAL:
                animation_step_normal_(dotstar, delta);
                animation_step_sparkles_(dotstar, delta);
                break;
            case GEM_MODE_LFO_PWM:
                animation_step_normal_(dotstar, delta);
                animation_step_sparkles_(dotstar, delta);
                break;
            case GEM_MODE_LFO_FM:
                animation_step_normal_(dotstar, delta);
                animation_step_sparkles_(dotstar, delta);
                break;
            case GEM_MODE_HARD_SYNC:
                animation_step_normal_(dotstar, delta);
                animation_step_sparkles_(dotstar, delta);
                break;
            case GEM_MODE_CALIBRATION:
                animation_step_calibration_(dotstar, ticks);
                break;
            default:
                break;
        }
    }

    gem_dotstar_update(dotstar);

    return true;
}

/* Private functions. */

static fix16_t noise(fix16_t phase) {
    return fix16_add(wntr_triangle(phase << 2), wntr_triangle(fix16_mul(fix16_pi, phase)));
}

static void animation_step_transition_(const struct GemDotstarCfg* dotstar, uint32_t delta) {
    const fix16_t duration = F16(1200);
    const fix16_t dotstar_count_f16 = fix16_from_int(dotstar->count);
    const fix16_t overlap = F16(0.25);
    const fix16_t max_phase = fix16_add(F16(1.1), overlap);
    const fix16_t interval = fix16_div(F16(1), dotstar_count_f16);
    const fix16_t scale = fix16_div(F16(1), fix16_add(interval, overlap));

    phase_a_ += fix16_div(fix16_from_int(delta), duration);

    if (phase_a_ > max_phase) {
        transitioning_ = false;
        return;
    }

    for (size_t i = 0; i < dotstar->count; i++) {
        size_t i_f16 = fix16_from_int(i);
        size_t offset = fix16_div(i_f16, dotstar_count_f16);
        size_t phase_i = fix16_sub(phase_a_, offset);
        size_t factor = fix16_mul(phase_i, scale);

        uint8_t v = (uint8_t)MINMAX((fix16_to_int(fix16_mul(F16(255), factor))), 0, 255);
        uint32_t color = wntr_colorspace_hsv_to_rgb(hue_accum_, 255, 255 - v);

        size_t led_index = cfg_.vertical_pos_index[dotstar->count - 1 - i];
        gem_dotstar_set32(led_index, color);
    }
}

static fix16_t phase_sparkle_ = 0;

static void animation_step_sparkles_(const struct GemDotstarCfg* dotstar, uint32_t delta) {
    const fix16_t decay_rate = F16(0.2);
    const int32_t base_sat = 100;
    const int32_t val_factor = 3;
    const fix16_t flicker_factor = F16(25);
    const fix16_t flicker_freq = F16(800.0);

    uint32_t spawn_rate = 500 - fix16_to_int(fix16_mul(gem_led_inputs.lfo_gain, F16(400)));
    uint8_t decay_step = (uint8_t)(fix16_to_int(fix16_mul(fix16_from_int(delta), decay_rate)));

    phase_sparkle_ += fix16_div(fix16_from_int(delta), flicker_freq);
    fix16_t flicker_amount = noise(phase_sparkle_);

    for (size_t i = 0; i < dotstar->count; i++) {
        if (sparkles_[i] == 0 && wntr_random32() % spawn_rate == 0)
            sparkles_[i] = 255;

        if (sparkles_[i] == 0) {
            continue;
        }

        uint16_t hue = (hue_accum_ + cfg_.hue_offsets[i]) % UINT16_MAX;
        uint8_t sat = (uint8_t)MINMAX(-base_sat + (255 - sparkles_[i]), 0, 255);
        int32_t val = MIN(sparkles_[i] * val_factor, 255);

        val += fix16_to_int(fix16_mul(flicker_amount, flicker_factor));
        val = MINMAX(val, 0, 255);

        uint32_t color = wntr_colorspace_hsv_to_rgb(hue, sat, (uint8_t)val);

        gem_dotstar_set32(i, color);

        if (sparkles_[i] <= decay_step)
            sparkles_[i] = 0;
        else
            sparkles_[i] -= decay_step;
    }
}

static void animation_step_normal_(const struct GemDotstarCfg* dotstar, uint32_t delta) {
    phase_a_ += fix16_div(fix16_from_int(delta), F16(2200.0));
    if (phase_a_ > F16(1.0))
        phase_a_ = fix16_sub(phase_a_, F16(1.0));

    hue_accum_ += delta * 5;

    for (size_t i = 0; i < dotstar->count; i++) {
        fix16_t phase_offset = fix16_div(fix16_from_int(i), fix16_from_int(dotstar->count));
        fix16_t sin_a = wntr_sine_normalized(phase_a_ + phase_offset);
        uint8_t value = 20 + fix16_to_int(fix16_mul(sin_a, F16(235)));
        uint16_t hue = mode_ == GEM_MODE_HARD_SYNC ? hue_accum_ : (hue_accum_ + cfg_.hue_offsets[i]) % UINT16_MAX;
        uint32_t color = wntr_colorspace_hsv_to_rgb(hue, 255, value);
        gem_dotstar_set32(i, color);
    }

    if (mode_ == GEM_MODE_LFO_PWM || mode_ == GEM_MODE_LFO_FM) {
        uint8_t val = 127 + fix16_to_int(fix16_mul(F16(127), gem_led_inputs.lfo_amplitude));
        uint16_t hue_a = (49151 * gem_led_inputs.lfo_mod_a) >> 12;
        uint16_t hue_b = (49151 * gem_led_inputs.lfo_mod_b) >> 12;
        uint32_t color_a = wntr_colorspace_hsv_to_rgb(hue_a, 255, val);
        uint32_t color_b = wntr_colorspace_hsv_to_rgb(hue_b, 255, val);

        gem_dotstar_set32(mode_ == GEM_MODE_LFO_PWM ? cfg_.pwm_a_led : cfg_.fm_a_led, color_a);
        gem_dotstar_set32(mode_ == GEM_MODE_LFO_PWM ? cfg_.pwm_b_led : cfg_.fm_b_led, color_b);
    }
}

static void animation_step_calibration_(const struct GemDotstarCfg* dotstar, uint32_t ticks) {
    fix16_t bright_time = fix16_div(fix16_from_int(ticks / 2), F16(2000.0));
    fix16_t sinv = wntr_sine_normalized(bright_time);
    uint8_t value = fix16_to_int(fix16_mul(F16(255.0), sinv));
    uint32_t colora = wntr_colorspace_hsv_to_rgb(50000, 255, value);
    uint32_t colorb = wntr_colorspace_hsv_to_rgb(10000, 255, 255 - value);

    for (uint8_t i = 0; i < dotstar->count; i++) {
        if (i % 2 == 0) {
            gem_dotstar_set32(i, colora);
        } else {
            gem_dotstar_set32(i, colorb);
        }
    }
}

static void animation_step_tweak_(const struct GemDotstarCfg* dotstar, uint32_t delta) {
    (void)delta;

    for (uint8_t i = 0; i < dotstar->count; i++) { gem_dotstar_set32(i, 0); }

    uint8_t lfo_val = 127 + fix16_to_int(fix16_mul(F16(127), gem_led_inputs.lfo_amplitude));
    gem_dotstar_set32(cfg_.lfo_tweak_led, wntr_colorspace_hsv_to_rgb(49016, 255, lfo_val));

    uint16_t pitch_hue;
    uint8_t pitch_val;

    if (gem_led_inputs.pitch_tweak_a == UINT16_MAX) {
        pitch_hue = 0;
        pitch_val = 0;
    } else if (gem_led_inputs.pitch_tweak_a < 2048) {
        pitch_hue = 49016;
        pitch_val = 255 - (gem_led_inputs.pitch_tweak_a >> 3);
    } else {
        pitch_hue = 0;
        pitch_val = (gem_led_inputs.pitch_tweak_a - 2048) >> 3;
    }
    gem_dotstar_set32(cfg_.pitch_a_tweak_led, wntr_colorspace_hsv_to_rgb(pitch_hue, 255, pitch_val));

    if (gem_led_inputs.pitch_tweak_b == UINT16_MAX) {
        pitch_hue = 0;
        pitch_val = 0;
    } else if (gem_led_inputs.pitch_tweak_b < 2048) {
        pitch_hue = 49016;
        pitch_val = 255 - (gem_led_inputs.pitch_tweak_b >> 3);
    } else {
        pitch_hue = 0;
        pitch_val = (gem_led_inputs.pitch_tweak_b - 2048) >> 3;
    }
    gem_dotstar_set32(cfg_.pitch_b_tweak_led, wntr_colorspace_hsv_to_rgb(pitch_hue, 255, pitch_val));
}
