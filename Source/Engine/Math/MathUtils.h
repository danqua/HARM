#pragma once

#include "Engine/Core/Types.h"

namespace Hx {

    constexpr f32 Pi32 = 3.14159265358979323846f;

    inline f32 Abs(f32 value) {
        return (value < 0.0f) ? -value : value;
    }

    inline f32 Radians(f32 deg) {
        return deg * (Pi32 / 180.0f);
    }

    inline f32 Degrees(f32 rad) {
        return rad * (180.0f / Pi32);
    }

    inline f32 Sin(f32 value) {
        return sinf(value);
    }

    inline f32 Cos(f32 value) {
        return cosf(value);
    }

    inline f32 Tan(f32 value) {
        return tanf(value);
    }

    inline f32 ATan2(f32 Y, f32 X) {
        return atan2f(Y, X);
    }

}