#include "gem_button.h"
#include "gem_gpio.h"
#include "gem_systick.h"

/* Constants and macros */

#define DEBOUNCE_TRESHOLD 2 /* Milliseconds */
#define HOLD_THRESHOLD 200 /* Miliseconds */

/* Public functions */

void gem_button_init(struct gem_button* button){
    gem_gpio_set_as_input(button->port, button->pin, true);
}


void gem_button_update(struct gem_button* button) {
    button->_previous_state = button->state;
    /* Invert gpio reading since pressed means pulled to ground. */
    button->state = !gem_gpio_get(button->port, button->pin);

    if(button->state == true && button->_previous_state == false) {
        button->_rising_edge_time = gem_get_ticks();
    }

    button->_update_time = gem_get_ticks();
}


bool gem_button_tapped(struct gem_button* button) {
    /* The button is considered "tapped" if the last
       state is true, the current state is false, and
       the rising edge time is less than the hold
       treshold. */
    if(button->state == false
        && button->_previous_state == true
        && button->_update_time - button->_rising_edge_time > DEBOUNCE_TRESHOLD
        && button->_update_time - button->_rising_edge_time < HOLD_THRESHOLD) {
        return true;
    }
    return false;
}


bool gem_button_held(struct gem_button* button) {
    /* The button is considered "held" if the
       state is true and the rising edge time is
       greater than the hold threshold. */
    if(button->state == true && button->_update_time - button->_rising_edge_time > HOLD_THRESHOLD) {
        return true;
    }
    return false;
}