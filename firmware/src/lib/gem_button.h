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
    bool _hold_barrier;
};


void gem_button_init(struct gem_button* button);

void gem_button_update(struct gem_button* button);

bool gem_button_tapped(struct gem_button* button);

bool gem_button_held(struct gem_button* button);

/*
    These functions check if the button has just started being held
    or just stopped being held. They modify the button state, so
    it's only valid to call these once per hold/release cycle.
*/
bool gem_button_start_hold(struct gem_button* button);

bool gem_button_end_hold(struct gem_button* button);