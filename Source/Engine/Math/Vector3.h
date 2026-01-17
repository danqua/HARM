#pragma once

#include "Engine/Core/Types.h"
#include <cmath>

namespace Hx {

    struct Vector3 {
        f32 x;
        f32 y;
        f32 z;
    
        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

        Vector3(f32 a, f32 b, f32 c) : x(a), y(b), z(c) {}

        Vector3(f32 scalar) : x(scalar), y(scalar), z(scalar) {}

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

        Vector3& operator*=(f32 scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        Vector3& operator/=(f32 scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        Vector3 operator-() const {
            return Vector3(-x, -y, -z);
        }

        Vector3 operator+(const Vector3& V) const {
            Vector3 result(x + V.x, y + V.y, z + V.z);
            return result;
        }

        Vector3 operator-(const Vector3& V) const {
            Vector3 result(x - V.x, y - V.y, z - V.z);
            return result;
        }

        Vector3 operator*(f32 scalar) const {
            Vector3 result(x * scalar, y * scalar, z * scalar);
            return result;
        }

        Vector3 operator/(f32 scalar) const {
            Vector3 result(x / scalar, y / scalar, z / scalar);
            return result;
        }

        static Vector3 Zero() {
            return Vector3(0.0f, 0.0f, 0.0f);
        }

        static Vector3 One() {
            return Vector3(1.0f, 1.0f, 1.0f);
        }
    };

    inline f32 Dot(const Vector3& A, const Vector3& B) {
        f32 result = A.x * B.x + A.y * B.y + A.z * B.z;
        return result;
    }

    inline f32 Length(const Vector3& V) {
        f32 len = V.x * V.x + V.y * V.y + V.z * V.z;
        f32 result = sqrtf(len);
        return result;
    }

    inline Vector3 Normalize(const Vector3& V) {
        f32 len = Length(V);
        if (len > 0) {
            Vector3 result;
            result.x = V.x / len;
            result.y = V.y / len;
            result.z = V.z / len;
            return result;
        }
        return Vector3{ 0.0f, 0.0f, 0.0f };    
    }

    inline Vector3 Cross(const Vector3& A, const Vector3& B) {
        Vector3 result;
        result.x = A.y * B.z - A.z * B.y;
        result.y = A.z * B.x - A.x * B.z;
        result.z = A.x * B.y - A.y * B.x;
        return result;
    }

}