#pragma once

#include "Engine/Core/Types.h"
#include <cmath>

namespace Engine::Math {

    struct Vector3 {
        f32 x;
        f32 y;
        f32 z;
    
        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

        Vector3(f32 a, f32 b, f32 c) : x(a), y(b), z(c) {}

        Vector3(f32 Scalar) : x(Scalar), y(Scalar), z(Scalar) {}

        Vector3(const Vector3& V) : x(V.x), y(V.y), z(V.z) {}

        bool operator==(const Vector3& V) const {
            bool Result = (x == V.x) && (y == V.y) && (z == V.z);
            return Result;
        }

        Vector3& operator+=(const Vector3& V) {
            x += V.x;
            y += V.y;
            z += V.z;
            return *this;
        }

        Vector3& operator-=(const Vector3& V) {
            x -= V.x;
            y -= V.y;
            z -= V.z;
            return *this;
        }

        Vector3& operator*=(f32 Scalar) {
            x *= Scalar;
            y *= Scalar;
            z *= Scalar;
            return *this;
        }

        Vector3& operator/=(f32 Scalar) {
            x /= Scalar;
            y /= Scalar;
            z /= Scalar;
            return *this;
        }

        Vector3 operator-() const {
            return Vector3(-x, -y, -z);
        }

        Vector3 operator+(const Vector3& V) const {
            Vector3 Result(x + V.x, y + V.y, z + V.z);
            return Result;
        }

        Vector3 operator-(const Vector3& V) const {
            Vector3 Result(x - V.x, y - V.y, z - V.z);
            return Result;
        }

        Vector3 operator*(f32 Scalar) const {
            Vector3 Result(x * Scalar, y * Scalar, z * Scalar);
            return Result;
        }

        Vector3 operator/(f32 Scalar) const {
            Vector3 Result(x / Scalar, y / Scalar, z / Scalar);
            return Result;
        }
    };

    inline f32 Length(const Vector3& V) {
        f32 Len = V.x * V.x + V.y * V.y + V.z * V.z;
        f32 Result = sqrtf(Len);
        return Result;
    }

    inline Vector3 Normalize(const Vector3& V) {
        f32 Len = Length(V);
        if (Length > 0) {
            Vector3 Result;
            Result.x = V.x / Len;
            Result.y = V.y / Len;
            Result.z = V.z / Len;
            return Result;
        }
        return Vector3{ 0.0f, 0.0f, 0.0f };    
    }

}