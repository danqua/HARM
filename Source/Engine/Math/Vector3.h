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

        Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}

        bool operator==(const Vector3& v) const {
            bool result = (x == v.x) && (y == v.y) && (z == v.z);
            return result;
        }

        Vector3& operator+=(const Vector3& v) {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        Vector3& operator-=(const Vector3& v) {
            x -= v.x;
            y -= v.y;
            z -= v.z;
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

        Vector3 operator+(const Vector3& v) const {
            Vector3 result(x + v.x, y + v.y, z + v.z);
            return result;
        }

        Vector3 operator-(const Vector3& v) const {
            Vector3 result(x - v.x, y - v.y, z - v.z);
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

    inline f32 Dot(const Vector3& a, const Vector3& b) {
        f32 result = a.x * b.x + a.y * b.y + a.z * b.z;
        return result;
    }

    inline f32 Length(const Vector3& v) {
        f32 len = v.x * v.x + v.y * v.y + v.z * v.z;
        f32 result = sqrtf(len);
        return result;
    }

    inline Vector3 Normalize(const Vector3& v) {
        f32 len = Length(v);
        if (len > 0) {
            Vector3 result;
            result.x = v.x / len;
            result.y = v.y / len;
            result.z = v.z / len;
            return result;
        }
        return Vector3{ 0.0f, 0.0f, 0.0f };    
    }

    inline Vector3 Cross(const Vector3& a, const Vector3& b) {
        Vector3 result;
        result.x = a.y * b.z - a.z * b.y;
        result.y = a.z * b.x - a.x * b.z;
        result.z = a.x * b.y - a.y * b.x;
        return result;
    }

}