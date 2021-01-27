/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_led_animation.h"
#include "fix16.h"
#include "gem_colorspace.h"
#include "gem_config.h"
#include "gem_dotstar.h"
#include "gem_random.h"
#include "gem_systick.h"
#include "gem_waveforms.h"
#include <stdint.h>

struct GemLEDTweakData gem_led_tweak_data = {.lfo_value = F16(0)};

static const uint32_t hue_offsets_[GEM_DOTSTAR_COUNT] = {
    65355 / GEM_DOTSTAR_COUNT * 2,
    65355 / GEM_DOTSTAR_COUNT * 2,
    65355 / GEM_DOTSTAR_COUNT * 6,
    0,
    65355 / GEM_DOTSTAR_COUNT,
    65355 / GEM_DOTSTAR_COUNT * 4,
    65355 / GEM_DOTSTAR_COUNT * 3,
};
static enum GemLEDAnimationMode mode_ = GEM_LED_MODE_NORMAL;
static uint32_t last_update_;
static fix16_t phase_a_ = F16(0);
static uint32_t hue_accum_;
static uint8_t sparkles_[GEM_DOTSTAR_COUNT];

/* Forward declarations. */

static void animation_step_normal(uint32_t delta);
static void animation_step_hard_sync(uint32_t delta);
static void animation_step_calibration(uint32_t ticks);
static void animation_step_tweak(uint32_t ticks);

/* Public functions. */

void gem_led_animation_init() { last_update_ = gem_get_ticks(); }

void gem_led_animation_set_mode(enum GemLEDAnimationMode mode) { mode_ = mode; }

bool gem_led_animation_step() {
    uint32_t ticks = gem_get_ticks();
    uint32_t delta = ticks - last_update_;
    if (delta < GEM_ANIMATION_INTERVAL) {
        return false;
    }

    last_update_ = ticks;

    switch (mode_) {
        case GEM_LED_MODE_NORMAL:
            animation_step_normal(delta);
            break;
        case GEM_LED_MODE_HARD_SYNC:
            animation_step_hard_sync(delta);
            break;
        case GEM_LED_MODE_CALIBRATION:
            animation_step_calibration(ticks);
            break;
        case GEM_LED_MODE_TWEAK:
            animation_step_tweak(delta);
            break;
        default:
            break;
    }
    gem_dotstar_update();

    return true;
}

/* Private functions. */

static void animation_step_normal(uint32_t delta) {
    phase_a_ += fix16_div(fix16_from_int(delta), F16(2200.0));
    if (phase_a_ > F16(1.0))
        phase_a_ = fix16_sub(phase_a_, F16(1.0));

    hue_accum_ += delta * 5;

    for (size_t i = 0; i < GEM_DOTSTAR_COUNT; i++) {
        fix16_t phase_offset = fix16_div(fix16_from_int(i), F16(GEM_DOTSTAR_COUNT));
        fix16_t sin_a = gem_sine_norm(phase_a_ + phase_offset);
        uint8_t value = 20 + fix16_to_int(fix16_mul(sin_a, F16(235)));
        uint16_t hue = (hue_accum_ + hue_offsets_[i]) % UINT16_MAX;
        uint32_t color;

        if (gem_random32() % 400 == 0)
            sparkles_[i] = 255;

        if (sparkles_[i] == 0) {
            color = gem_colorspace_hsv_to_rgb(hue, 255, value);
        } else {
            color = gem_colorspace_hsv_to_rgb(hue, 255 - sparkles_[i], value);
            if (sparkles_[i] <= delta / 4)
                sparkles_[i] = 0;
            else
                sparkles_[i] -= delta / 4;
        }

        gem_dotstar_set32(i, color);
    }
}

static void animation_step_hard_sync(uint32_t delta) {
    phase_a_ += fix16_div(fix16_from_int(delta), F16(2200.0));
    if (phase_a_ > F16(1.0))
        phase_a_ = fix16_sub(phase_a_, F16(1.0));

    hue_accum_ += delta * 5;
    uint16_t hue = hue_accum_ % UINT16_MAX;

    for (size_t i = 0; i < GEM_DOTSTAR_COUNT; i++) {
        fix16_t phase_offset = fix16_div(fix16_from_int(i), F16(GEM_DOTSTAR_COUNT));
        fix16_t sin_a = gem_sine_norm(phase_a_ + phase_offset);
        uint8_t value = 20 + fix16_to_int(fix16_mul(sin_a, F16(235)));
        uint32_t color;

        if (gem_random32() % 400 == 0)
            sparkles_[i] = 255;

        if (sparkles_[i] == 0) {
            color = gem_colorspace_hsv_to_rgb(hue, 255, value);
        } else {
            color = gem_colorspace_hsv_to_rgb(hue, 255 - sparkles_[i], value);
            if (sparkles_[i] <= delta / 4)
                sparkles_[i] = 0;
            else
                sparkles_[i] -= delta / 4;
        }

        gem_dotstar_set32(i, color);
    }
}

static void animation_step_calibration(uint32_t ticks) {
    fix16_t bright_time = fix16_div(fix16_from_int(ticks / 2), F16(2000.0));
    fix16_t sinv = gem_sine_norm(bright_time);
    uint8_t value = fix16_to_int(fix16_mul(F16(255.0), sinv));
    uint32_t colora = gem_colorspace_hsv_to_rgb(50000, 255, value);
    uint32_t colorb = gem_colorspace_hsv_to_rgb(10000, 255, 255 - value);

    for (uint8_t i = 0; i < GEM_DOTSTAR_COUNT; i++) {
        if (i % 2 == 0) {
            gem_dotstar_set32(i, colora);
        } else {
            gem_dotstar_set32(i, colorb);
        }
    }
}

static void animation_step_tweak(uint32_t delta) {
    hue_accum_ += delta;

    for (uint8_t i = 0; i < GEM_DOTSTAR_COUNT; i++) { gem_dotstar_set32(i, 0); }

    if (gem_led_tweak_data.castor_pwm) {
        gem_dotstar_set(0, 0, 255, 255);
        gem_dotstar_set(1, 0, 255, 255);
    }

    if (gem_led_tweak_data.pollux_pwm) {
        gem_dotstar_set(2, 255, 0, 255);
        gem_dotstar_set(3, 255, 0, 255);
    }

    fix16_t lfoadj = fix16_div(fix16_add(gem_led_tweak_data.lfo_value, F16(1.0)), F16(2.0));
    uint8_t lfo_value = fix16_to_int(fix16_mul(F16(255.0), lfoadj));
    gem_dotstar_set32(4, gem_colorspace_hsv_to_rgb(UINT16_MAX / 12 * 2, 255, lfo_value));
    gem_dotstar_set32(5, gem_colorspace_hsv_to_rgb(UINT16_MAX / 12 * 2, 255, lfo_value));
    gem_dotstar_set32(6, gem_colorspace_hsv_to_rgb(UINT16_MAX / 12 * 2, 255, lfo_value));
}
