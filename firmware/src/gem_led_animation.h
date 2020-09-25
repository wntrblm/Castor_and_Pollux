#pragma once

enum gem_led_animation_mode { GEM_LED_MODE_NORMAL, GEM_LED_MODE_CALIBRATION };

void gem_led_animation_init();

void gem_led_animation_step();

void gem_led_animation_set_mode(enum gem_led_animation_mode mode);