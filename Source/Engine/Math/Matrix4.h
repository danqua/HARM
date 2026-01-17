#pragma once

#include "Engine/Core/Types.h"
#include "Engine/Math/Vector3.h"

#include <cmath>

namespace Hx {

    struct Matrix4 {
        f32 m[16];

        static inline Matrix4 Identity() {
            Matrix4 Result = {};
            Result.m[0] = 1.0f;
            Result.m[5] = 1.0f;
            Result.m[10] = 1.0f;
            Result.m[15] = 1.0f;
            return Result;
        }
    };

    inline Matrix4 Perspective(f32 Fov, f32 Aspect, f32 Near, f32 Far) {
        Matrix4 result = {};
        f32 tanHalfFov = tanf(Fov / 2.0f);

        result.m[0] = 1.0f / (Aspect * tanHalfFov);
        result.m[5] = 1.0f / (tanHalfFov);
        result.m[10] = -(Far + Near) / (Far - Near);
        result.m[11] = -1.0f;
        result.m[14] = -(2.0f * Far * Near) / (Far - Near);

        return result;
    }

    inline Matrix4 Orthographic(f32 Width, f32 Height, f32 Near, f32 Far) {
        Matrix4 result = {};
        result.m[0] = 2.0f / Width;
        result.m[5] = 2.0f / Height;
        result.m[10] = -2.0f / (Far - Near);
        result.m[12] = 0.0f;
        result.m[13] = 0.0f;
        result.m[14] = -(Far + Near) / (Far - Near);
        result.m[15] = 1.0f;
        return result;
    }

    inline Matrix4 LookAt(const Vector3& Eye, const Vector3& Center, const Vector3& Up) {
        Vector3 f = Normalize(Center - Eye);
        Vector3 s = Normalize(Vector3(
            f.y * Up.z - f.z * Up.y,
            f.z * Up.x - f.x * Up.z,
            f.x * Up.y - f.y * Up.x));
        Vector3 u = Vector3(
            s.y * f.z - s.z * f.y,
            s.z * f.x - s.x * f.z,
            s.x * f.y - s.y * f.x);

        Matrix4 result = {};
        result.m[0] = s.x;
        result.m[4] = s.y;
        result.m[8] = s.z;
        result.m[1] = u.x;
        result.m[5] = u.y;
        result.m[9] = u.z;
        result.m[2] = -f.x;
        result.m[6] = -f.y;
        result.m[10] = -f.z;
        result.m[15] = 1.0f;
        result.m[12] = -Dot(s, Eye);
        result.m[13] = -Dot(u, Eye);
        result.m[14] = Dot(f, Eye);

        return result;
    }

    inline Matrix4 Translate(const Vector3& Translation) {
        Matrix4 result = Matrix4::Identity();
        result.m[12] = Translation.x;
        result.m[13] = Translation.y;
        result.m[14] = Translation.z;
        return result;
    }

    inline Matrix4 Translate(const Matrix4& M, const Vector3& Translation) {
        Matrix4 result = M;
        result.m[12] += Translation.x;
        result.m[13] += Translation.y;
        result.m[14] += Translation.z;
        return result;
    }

    inline Matrix4 Scale(const Vector3& Scaling) {
        Matrix4 result = Matrix4::Identity();
        result.m[0] = Scaling.x;
        result.m[5] = Scaling.y;
        result.m[10] = Scaling.z;
        return result;
    }

    inline Matrix4 Scaler(const Matrix4& M, const Vector3& Scaling) {
        Matrix4 result = M;
        result.m[0] *= Scaling.x;
        result.m[5] *= Scaling.y;
        result.m[10] *= Scaling.z;
        return result;
    }

}
