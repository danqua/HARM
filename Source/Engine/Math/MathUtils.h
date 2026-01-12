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
}