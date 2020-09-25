#include "gem_led_animation.h"
#include "fix16.h"
#include "gem_colorspace.h"
#include "gem_config.h"
#include "gem_dotstar.h"
#include "gem_systick.h"
#include "gem_waveforms.h"
#include <stdint.h>

enum gem_led_animation_mode _mode = GEM_LED_MODE_NORMAL;

void gem_led_animation_init() {}

void _gem_led_animation_step_normal() {
    uint32_t ticks = gem_get_ticks();
    for (uint8_t i = 0; i < GEM_DOTSTAR_COUNT; i++) {
        // sinadj = (sine(bright_time) + 1.0) / 2.0
        fix16_t bright_time = fix16_div(fix16_from_int(ticks * i / 2), F16(5000.0f));
        fix16_t sinv = gem_sine(bright_time);
        fix16_t sinadj = fix16_div(fix16_add(sinv, F16(1.0f)), F16(2.0f));
        // value = 255 * sinadj
        uint8_t value = fix16_to_int(fix16_mul(F16(255.0f), sinadj));
        uint32_t color =
            gem_colorspace_hsv_to_rgb((ticks * 5) + (65535 / GEM_DOTSTAR_COUNT * i), 255 - value / 2, value);
        gem_dotstar_set32(i, color);
    }
}

void _gem_led_animation_step_calibration() {
    uint32_t ticks = gem_get_ticks();
    for (uint8_t i = 0; i < GEM_DOTSTAR_COUNT; i++) {
        fix16_t bright_time = fix16_div(fix16_from_int(ticks / 2), F16(5000.0f));
        fix16_t sinv = gem_sine(bright_time);
        fix16_t sinadj = fix16_div(fix16_add(sinv, F16(1.0f)), F16(2.0f));
        uint8_t value = fix16_to_int(fix16_mul(F16(255.0f), sinadj));
        uint32_t color = gem_colorspace_hsv_to_rgb(0, 255 - value / 2, value);
        gem_dotstar_set32(i, color);
    }
}

void gem_led_animation_step() {
    switch (_mode) {
        case GEM_LED_MODE_NORMAL:
            _gem_led_animation_step_normal();
            break;
        case GEM_LED_MODE_CALIBRATION:
            _gem_led_animation_step_calibration();
            break;
        default:
            break;
    }
    gem_dotstar_update();
}

void gem_led_animation_set_mode(enum gem_led_animation_mode mode) { _mode = mode; }