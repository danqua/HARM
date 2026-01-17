#pragma once

#include "Engine/Core/Types.h"

namespace Hx {

    struct Vector4 {
        f32 x;
        f32 y;
        f32 z;
        f32 w;

        Vector4() : x(0), y(0), z(0), w(0) {}
        Vector4(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w) {}
        Vector4(f32 scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
        Vector4(const Vector4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
    };

}