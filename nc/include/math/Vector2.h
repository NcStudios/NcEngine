#pragma once
#include "Math.h"

namespace nc
{
    struct Vector2
    {
        float x, y;

        constexpr explicit Vector2() noexcept : x{0.0f}, y{0.0f} {}
        constexpr explicit Vector2(float X, float Y) noexcept : x{X}, y{Y} {}

        static constexpr Vector2 Splat(float v) noexcept { return Vector2{v, v}; }
        static constexpr Vector2 Zero()  noexcept { return Vector2{ 0, 0}; }
        static constexpr Vector2 One()   noexcept { return Vector2{ 1, 1}; }
        static constexpr Vector2 Up()    noexcept { return Vector2{ 1, 0}; }
        static constexpr Vector2 Down()  noexcept { return Vector2{-1, 0}; }
        static constexpr Vector2 Left()  noexcept { return Vector2{ 0,-1}; }
        static constexpr Vector2 Right() noexcept { return Vector2{ 0, 1}; }
    };

    constexpr bool operator ==(const Vector2& lhs, const Vector2& rhs) noexcept;
    constexpr bool operator !=(const Vector2& lhs, const Vector2& rhs) noexcept;
    constexpr Vector2 operator +(const Vector2& lhs, const Vector2& rhs) noexcept;
    constexpr Vector2 operator -(const Vector2& lhs, const Vector2& rhs) noexcept;
    constexpr Vector2 operator *(const Vector2& vec, float scalar) noexcept;
    constexpr Vector2 operator *(float scalar, const Vector2& vec) noexcept;
    constexpr Vector2 operator /(const Vector2& vec, float scalar) noexcept;
    constexpr Vector2 HadamardProduct(const Vector2& lhs, const Vector2& rhs) noexcept;
    constexpr float SquareMagnitude(const Vector2& vec) noexcept;
    constexpr float Magnitude(const Vector2& vec) noexcept;
    constexpr Vector2 Normalize(const Vector2& vec) noexcept;
    constexpr Vector2 Lerp(const Vector2& lhs, const Vector2& rhs, float factor) noexcept;
    constexpr float Dot(const Vector2& lhs, const Vector2& rhs) noexcept;
    constexpr float Distance(const Vector2& lhs, const Vector2& rhs) noexcept;

    constexpr bool operator ==(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return math::FloatEqual(lhs.x, rhs.x) && math::FloatEqual(lhs.y, rhs.y);
    }

    constexpr bool operator !=(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr Vector2 operator +(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    constexpr Vector2 operator -(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    constexpr Vector2 operator *(const Vector2& vec, float scalar) noexcept
    {
        return Vector2(vec.x * scalar, vec.y * scalar);
    }

    constexpr Vector2 operator *(float scalar, const Vector2& vec) noexcept
    {
        return vec * scalar;
    }

    constexpr Vector2 operator /(const Vector2& vec, float scalar) noexcept
    {
        return Vector2(vec.x / scalar, vec.y / scalar);
    }

    constexpr Vector2 HadamardProduct(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return Vector2{lhs.x * rhs.x, lhs.y * rhs.y};
    }

    constexpr float SquareMagnitude(const Vector2& vec) noexcept
    {
        return vec.x * vec.x + vec.y * vec.y;
    }

    constexpr float Magnitude(const Vector2& vec) noexcept
    {
        return sqrt(SquareMagnitude(vec));
    }

    constexpr Vector2 Normalize(const Vector2& vec) noexcept
    {
        return vec == Vector2::Zero() ? Vector2::Zero() : vec / Magnitude(vec);
    }

    constexpr Vector2 Lerp(const Vector2& from, const Vector2& to, float factor) noexcept
    { 
        return math::Lerp(from, to, factor);
    }

    constexpr float Dot(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    constexpr float Distance(const Vector2& lhs, const Vector2& rhs) noexcept
    {
        float x = lhs.x - rhs.x;
        float y = lhs.y - rhs.y;
        return sqrt(x * x + y * y);
    }
} // end namespace nc