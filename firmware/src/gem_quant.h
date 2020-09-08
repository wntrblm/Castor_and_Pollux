#pragma once
#include "math.h"
#include "stdint.h"
#include "gem_config.h"


inline static float _gem_quant(float delta, float value) { return delta * floor(value / delta * 0.5); }

inline static float gem_quant_pitch_cv(uint16_t value) {
    return _gem_quant(GEM_QUANT_CV_STEP, value);
}