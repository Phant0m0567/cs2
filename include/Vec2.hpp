#pragma once

#include <cmath>

class Vec2 {
public:
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
    Vec2 operator-(const Vec2& other) const { return {x - other.x, y - other.y}; }
    Vec2 operator*(float scalar) const { return {x * scalar, y * scalar}; }
    Vec2 operator/(float scalar) const { return {x / scalar, y / scalar}; }

    Vec2& operator+=(const Vec2& other) { x += other.x; y += other.y; return *this; }
    Vec2& operator-=(const Vec2& other) { x -= other.x; y -= other.y; return *this; }
    Vec2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    Vec2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

    Vec2 operator-() const { return {-x, -y}; }

    bool operator==(const Vec2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vec2& other) const { return !(*this == other); }

    float Dot(const Vec2& other) const { return x * other.x + y * other.y; }
    float LengthSqr() const { return Dot(*this); }
    float Length() const { return std::sqrt(LengthSqr()); }

    Vec2 Normalized() const {
        const float len = Length();
        if (len == 0.0f) {
            return *this;
        }
        return *this / len;
    }

    float Distance(const Vec2& other) const { return (*this - other).Length(); }
    float DistanceSqr(const Vec2& other) const { return (*this - other).LengthSqr(); }

    static Vec2 Lerp(const Vec2& from, const Vec2& to, float t) {
        return from + (to - from) * t;
    }
};

inline Vec2 operator*(float scalar, const Vec2& vec) {
    return vec * scalar;
}
