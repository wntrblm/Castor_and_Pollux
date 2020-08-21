#pragma once

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

inline static float lerpf(float a, float b, float t) {
    if (t == 0.0f)
        return a;
    if (t == 1.0f)
        return b;
    float x = a + t * (b - a);
    return (t > 1) == (b > a) ? MAX(b, x) : MIN(b, x);
}