/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_button.h"
#include "wntr_gpio.h"
#include "wntr_ticks.h"

/* Constants and macros */

#define DEBOUNCE_TRESHOLD 2 /* Milliseconds */
#define HOLD_THRESHOLD 500  /* Miliseconds */

/* Public functions */

void WntrButton_init(struct WntrButton* button) { wntr_gpio_set_as_input(button->port, button->pin, true); }

void WntrButton_update(struct WntrButton* button) {
    button->_previous_state = button->state;
    /* Invert gpio reading since pressed means pulled to ground. */
    button->state = !wntr_gpio_get(button->port, button->pin);

    if (button->state == true && button->_previous_state == false) {
        button->_rising_edge_time = wntr_ticks();
    }

    button->_update_time = wntr_ticks();
}

bool WntrButton_tapped(struct WntrButton* button) {
    /* The button is considered "tapped" if the last
       state is true, the current state is false, and
       the rising edge time is less than the hold
       treshold. */
    if (button->state == false && button->_previous_state == true &&
        button->_update_time - button->_rising_edge_time > DEBOUNCE_TRESHOLD &&
        button->_update_time - button->_rising_edge_time < HOLD_THRESHOLD) {
        return true;
    }
    return false;
}

bool WntrButton_held(struct WntrButton* button) {
    /* The button is considered "held" if the
       state is true and the rising edge time is
       greater than the hold threshold. */
    if (button->state == true && button->_update_time - button->_rising_edge_time > HOLD_THRESHOLD) {
        return true;
    }
    return false;
}

bool WntrButton_hold_started(struct WntrButton* button) {
    if (WntrButton_held(button) && !button->_hold_barrier) {
        button->_hold_barrier = true;
        return true;
    }
    return false;
}

bool WntrButton_hold_ended(struct WntrButton* button) {
    if (!WntrButton_held(button) && button->_hold_barrier) {
        button->_hold_barrier = false;
        return true;
    }
    return false;
}
