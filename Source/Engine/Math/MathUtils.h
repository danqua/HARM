#pragma once

#include "Engine/Core/Types.h"

namespace Engine::Math {

    constexpr f32 Pi32 = 3.14159265358979323846f;

    inline f32 Radians(f32 Deg) {
        return Deg * (Pi32 / 180.0f);
    }

    inline f32 Degrees(f32 Rad) {
        return Rad * (180.0f / Pi32);
    }

    inline f32 Sin(f32 Value) {
        return sinf(Value);
    }

    inline f32 Cos(f32 Value) {
        return cosf(Value);
    }

    inline f32 Tan(f32 Value) {
        return tanf(Value);
    }

    inline f32 ATan2(f32 Y, f32 X) {
        return atan2f(Y, X);
    }

}