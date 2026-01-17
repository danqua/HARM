#pragma once

#include "Engine/Core/Types.h"
#include "Engine/Math/Vector3.h"

#include <cmath>

namespace Hx {

    struct Matrix4 {
        f32 m[16];

        static inline Matrix4 Identity() {
            Matrix4 result = {};
            result.m[0] = 1.0f;
            result.m[5] = 1.0f;
            result.m[10] = 1.0f;
            result.m[15] = 1.0f;
            return result;
        }
    };

    inline Matrix4 Perspective(f32 fov, f32 aspect, f32 near, f32 far) {
        Matrix4 result = {};
        f32 tanHalfFov = tanf(fov / 2.0f);

        result.m[0] = 1.0f / (aspect * tanHalfFov);
        result.m[5] = 1.0f / (tanHalfFov);
        result.m[10] = -(far + near) / (far - near);
        result.m[11] = -1.0f;
        result.m[14] = -(2.0f * far * near) / (far - near);

        return result;
    }

    inline Matrix4 Orthographic(f32 width, f32 height, f32 near, f32 far) {
        Matrix4 result = {};
        result.m[0] = 2.0f / width;
        result.m[5] = 2.0f / height;
        result.m[10] = -2.0f / (far - near);
        result.m[12] = 0.0f;
        result.m[13] = 0.0f;
        result.m[14] = -(far + near) / (far - near);
        result.m[15] = 1.0f;
        return result;
    }

    inline Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
        Vector3 f = Normalize(center - eye);
        Vector3 s = Normalize(Vector3(
            f.y * up.z - f.z * up.y,
            f.z * up.x - f.x * up.z,
            f.x * up.y - f.y * up.x));
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
        result.m[12] = -Dot(s, eye);
        result.m[13] = -Dot(u, eye);
        result.m[14] = Dot(f, eye);

        return result;
    }

    inline Matrix4 Translate(const Vector3& translation) {
        Matrix4 result = Matrix4::Identity();
        result.m[12] = translation.x;
        result.m[13] = translation.y;
        result.m[14] = translation.z;
        return result;
    }

    inline Matrix4 Translate(const Matrix4& m, const Vector3& translation) {
        Matrix4 result = m;
        result.m[12] += translation.x;
        result.m[13] += translation.y;
        result.m[14] += translation.z;
        return result;
    }

    inline Matrix4 Scale(const Vector3& scaling) {
        Matrix4 result = Matrix4::Identity();
        result.m[0] = scaling.x;
        result.m[5] = scaling.y;
        result.m[10] = scaling.z;
        return result;
    }

    inline Matrix4 Scaler(const Matrix4& m, const Vector3& scaling) {
        Matrix4 result = m;
        result.m[0] *= scaling.x;
        result.m[5] *= scaling.y;
        result.m[10] *= scaling.z;
        return result;
    }

}
