#pragma once

/* Helper for detecting button interactions - tap, hold, etc. */

#include "stdbool.h"
#include "stdint.h"

struct GemButton {
    uint8_t port;
    uint8_t pin;
    bool state;
    bool _previous_state;
    uint32_t _rising_edge_time;
    uint32_t _update_time;
    bool _hold_barrier;
};


void GemButton_init(struct GemButton* button);

void GemButton_update(struct GemButton* button);

bool GemButton_tapped(struct GemButton* button);

bool GemButton_held(struct GemButton* button);

/*
    These functions check if the button has just started being held
    or just stopped being held. They modify the button state, so
    it's only valid to call these once per hold/release cycle.
*/
bool GemButton_hold_started(struct GemButton* button);

bool GemButton_hold_ended(struct GemButton* button);
