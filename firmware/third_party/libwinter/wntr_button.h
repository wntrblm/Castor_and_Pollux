/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/*
    Helper for detecting various button interactions such as tap, hold, etc.

    This code assumes that the button connects the GPIO pin to ground when
    pressed- that is, the logic is inverted here. A low input means the button
    is pressed and a high input means the button isn't.
*/

#include "stdbool.h"
#include "stdint.h"

struct WntrButton {
    uint8_t port;
    uint8_t pin;
    bool state;
    bool _previous_state;
    uint32_t _rising_edge_time;
    uint32_t _update_time;
    bool _hold_barrier;
};

/*
    Configures the button's GPIO pins and sets initial state.
*/
void WntrButton_init(struct WntrButton* button);

/*
    Should be called during the update loop before checking any of the
    interactions.
*/
void WntrButton_update(struct WntrButton* button);

/*
    Returns true if the button was "tapped": pressed and released within a
    short period of time.
*/
bool WntrButton_tapped(struct WntrButton* button);

/*
    Returns true if the button is being held down.
*/
bool WntrButton_held(struct WntrButton* button);

/*
    These functions check if the button has just started being held
    or just stopped being held. They modify the button state, so
    it's only valid to call these once per hold/release cycle.
*/
bool WntrButton_hold_started(struct WntrButton* button);

bool WntrButton_hold_ended(struct WntrButton* button);
