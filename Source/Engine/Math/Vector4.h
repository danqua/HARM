#pragma once

#include "Engine/Core/Types.h"

namespace Engine::Math {

    struct Vector4 {
        f32 x;
        f32 y;
        f32 z;
        f32 w;

        Vector4() : x(0), y(0), z(0), w(0) {}
        Vector4(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w) {}
        Vector4(f32 Scalar) : x(Scalar), y(Scalar), z(Scalar), w(Scalar) {}
        Vector4(const Vector4& V) : x(V.x), y(V.y), z(V.z), w(V.w) {}
    };

}