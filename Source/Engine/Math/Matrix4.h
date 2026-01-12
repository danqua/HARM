#pragma once

#include "Engine/Core/Types.h"
#include "Engine/Math/Vector3.h"

#include <cmath>

namespace Engine::Math {

    struct Matrix4 {
        f32 m[16];
    };
    

    inline Matrix4 Identity() {
        Matrix4 Result = {};
        Result.m[0] = 1.0f;
        Result.m[5] = 1.0f;
        Result.m[10] = 1.0f;
        Result.m[15] = 1.0f;
        return Result;
    }

    inline Matrix4 Perspective(f32 Fov, f32 Aspect, f32 Near, f32 Far) {
        Matrix4 Result = {};
        f32 TanHalfFov = tanf(Fov / 2.0f);

        Result.m[0] = 1.0f / (Aspect * TanHalfFov);
        Result.m[5] = 1.0f / (TanHalfFov);
        Result.m[10] = -(Far + Near) / (Far - Near);
        Result.m[11] = -1.0f;
        Result.m[14] = -(2.0f * Far * Near) / (Far - Near);

        return Result;
    }

    inline Matrix4 LookAt(const Vector3& Eye, const Vector3& Center, const Vector3& Up) {
        Vector3 F = Math::Normalize(Center - Eye);
        Vector3 S = Math::Normalize(Vector3(
            F.y * Up.z - F.z * Up.y,
            F.z * Up.x - F.x * Up.z,
            F.x * Up.y - F.y * Up.x));
        Vector3 U = Vector3(
            S.y * F.z - S.z * F.y,
            S.z * F.x - S.x * F.z,
            S.x * F.y - S.y * F.x);

        Matrix4 Result = {};
        Result.m[0] = S.x;
        Result.m[1] = U.x;
        Result.m[2] = -F.x;
        Result.m[4] = S.y;
        Result.m[5] = U.y;
        Result.m[6] = -F.y;
        Result.m[8] = S.z;
        Result.m[9] = U.z;
        Result.m[10] = -F.z;
        Result.m[15] = 1.0f;

        Result.m[12] = -(S.x * Eye.x + S.y * Eye.y + S.z * Eye.z);
        Result.m[13] = -(U.x * Eye.x + U.y * Eye.y + U.z * Eye.z);
        Result.m[14] = F.x * Eye.x + F.y * Eye.y + F.z * Eye.z;

        return Result;
    }

    inline Matrix4 Translate(const Vector3& Translation) {
        Matrix4 Result = Identity();
        Result.m[12] = Translation.x;
        Result.m[13] = Translation.y;
        Result.m[14] = Translation.z;
        return Result;
    }

    inline Matrix4 Translate(const Matrix4& M, const Vector3& Translation) {
        Matrix4 Result = M;
        Result.m[12] += Translation.x;
        Result.m[13] += Translation.y;
        Result.m[14] += Translation.z;
        return Result;
    }

    inline Matrix4 Scale(const Vector3& Scaling) {
        Matrix4 Result = Identity();
        Result.m[0] = Scaling.x;
        Result.m[5] = Scaling.y;
        Result.m[10] = Scaling.z;
        return Result;
    }

    inline Matrix4 Scaler(const Matrix4& M, const Vector3& Scaling) {
        Matrix4 Result = M;
        Result.m[0] *= Scaling.x;
        Result.m[5] *= Scaling.y;
        Result.m[10] *= Scaling.z;
        return Result;
    }

}