#pragma once

#include <cmath>

class Vec3 {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& other) const { return {x + other.x, y + other.y, z + other.z}; }
    Vec3 operator-(const Vec3& other) const { return {x - other.x, y - other.y, z - other.z}; }
    Vec3 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }
    Vec3 operator/(float scalar) const { return {x / scalar, y / scalar, z / scalar}; }

    Vec3& operator+=(const Vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    Vec3& operator-=(const Vec3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    Vec3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vec3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

    Vec3 operator-() const { return {-x, -y, -z}; }

    bool operator==(const Vec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    bool operator!=(const Vec3& other) const { return !(*this == other); }

    float Dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3 Cross(const Vec3& other) const {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    float LengthSqr() const { return Dot(*this); }
    float Length() const { return std::sqrt(LengthSqr()); }

    Vec3 Normalized() const {
        const float len = Length();
        if (len == 0.0f) {
            return *this;
        }
        return *this / len;
    }

    float Distance(const Vec3& other) const { return (*this - other).Length(); }
    float DistanceSqr(const Vec3& other) const { return (*this - other).LengthSqr(); }

    static Vec3 Lerp(const Vec3& from, const Vec3& to, float t) {
        return from + (to - from) * t;
    }
};

inline Vec3 operator*(float scalar, const Vec3& vec) {
    return vec * scalar;
}
