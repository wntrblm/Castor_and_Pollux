/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_button.h"
#include "gem_gpio.h"
#include "gem_systick.h"

/* Constants and macros */

#define DEBOUNCE_TRESHOLD 2 /* Milliseconds */
#define HOLD_THRESHOLD 200  /* Miliseconds */

/* Public functions */

void GemButton_init(struct GemButton* button) { gem_gpio_set_as_input(button->port, button->pin, true); }

void GemButton_update(struct GemButton* button) {
    button->_previous_state = button->state;
    /* Invert gpio reading since pressed means pulled to ground. */
    button->state = !gem_gpio_get(button->port, button->pin);

    if (button->state == true && button->_previous_state == false) {
        button->_rising_edge_time = gem_get_ticks();
    }

    button->_update_time = gem_get_ticks();
}

bool GemButton_tapped(struct GemButton* button) {
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

bool GemButton_held(struct GemButton* button) {
    /* The button is considered "held" if the
       state is true and the rising edge time is
       greater than the hold threshold. */
    if (button->state == true && button->_update_time - button->_rising_edge_time > HOLD_THRESHOLD) {
        return true;
    }
    return false;
}

bool GemButton_hold_started(struct GemButton* button) {
    if (GemButton_held(button) && !button->_hold_barrier) {
        button->_hold_barrier = true;
        return true;
    }
    return false;
}

bool GemButton_hold_ended(struct GemButton* button) {
    if (!GemButton_held(button) && button->_hold_barrier) {
        button->_hold_barrier = false;
        return true;
    }
    return false;
}
