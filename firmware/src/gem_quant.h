#pragma once
#include "gem_config.h"
#include "math.h"
#include "stdint.h"

inline static float _gem_quant(float delta, float value) { return delta * roundf(value / delta); }

inline static float gem_quant_pitch_cv(float value) { return _gem_quant(GEM_QUANT_CV_STEP, value); }