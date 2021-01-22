/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include <stdbool.h>
#include <stdint.h>

/*
    Routines for managing USB.

    These are thin wrappers over tinyUSB.
*/

void gem_usb_init();

void gem_usb_task();

bool gem_usb_midi_receive(uint8_t packet[4]);
bool gem_usb_midi_send(uint8_t packet[4]);
