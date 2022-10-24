/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Global configuration for all of Gemini's hardware and behavior. */

#include "gem_config_rev1.h"
#include "gem_config_rev5.h"
#include "sam.h"
#include "wntr_gpio.h"
#include <stdint.h>

/* Dotstar/animation constants */

#define GEM_MAX_DOTSTAR_COUNT 8
#define GEM_ANIMATION_INTERVAL 50

/* Hard sync button configuration. */

static const struct WntrGPIOPin button_pin_ = WNTR_GPIO_PIN(B, 8);

/* Behavioral constants. */

#define GEM_CV_DEFAULT_BASE_OFFSET F16(1.0)
#define GEM_TWEAK_MAX_LFO_FREQ F16(5)
