#pragma once

void gem_usb_init();

void gem_usb_task();

bool gem_usb_midi_receive(uint8_t packet[4]);