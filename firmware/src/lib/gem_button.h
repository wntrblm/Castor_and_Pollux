#pragma once

/* Helper for detecting button interactions - tap, hold, etc. */

#include "stdbool.h"
#include "stdint.h"

struct gem_button {
    uint8_t port;
    uint8_t pin;
    bool state;
    bool _previous_state;
    uint32_t _rising_edge_time;
    uint32_t _update_time;
};


void gem_button_init(struct gem_button* button);

void gem_button_update(struct gem_button* button);

bool gem_button_tapped(struct gem_button* button);

bool gem_button_held(struct gem_button* button);