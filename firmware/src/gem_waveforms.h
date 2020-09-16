#pragma once

#include "math.h"

float gem_sine(float phase) { return sinf(phase * 2.0f * 3.14569f); }

float gem_triangle(float phase) { return fmodf(phase * 2.0f, 1.0f) * 2.0f - 1.0f; }