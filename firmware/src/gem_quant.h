#pragma once
#include "math.h"
#include "stdint.h"

inline static float _gem_quant(float delta, float value) { return delta * floor(value / delta * 0.5); }

inline static float gem_quant_pitch_cv(uint16_t value) {
    /* TODO: Make this a config def. */
    /* 83.33 milivolts per semitone / 20 steps */
    return _gem_quant((0.0833f / 20.0f), value);
}