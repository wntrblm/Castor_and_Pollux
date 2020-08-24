#pragma once

void gem_usb_init();

void gem_usb_task();

bool gem_usb_midi_receive(uint8_t packet[4]);
bool gem_usb_midi_send(uint8_t packet[4]);
size_t gem_usb_midi_send_bytes(uint8_t* data, size_t len);