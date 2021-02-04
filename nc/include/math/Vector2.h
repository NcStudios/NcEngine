#pragma once
#include "Math.h"

struct ImVec2;

namespace nc
{
    struct Vector2
    {
        float x, y;

        constexpr explicit Vector2() noexcept : x{0.0f}, y{0.0f} {}
        constexpr explicit Vector2(float X, float Y) noexcept : x{X}, y{Y} {}
        Vector2(const ImVec2& vec) noexcept;
        Vector2(ImVec2&& vec) noexcept;
        Vector2& operator=(const ImVec2& vec) noexcept;
        Vector2& operator=(ImVec2&& vec) noexcept;
        ImVec2 ToImVec2() noexcept;

        static constexpr Vector2 Splat(float v) noexcept { return Vector2{v, v}; }
        static constexpr Vector2 Zero()  noexcept { return Vector2{ 0, 0}; }
        static constexpr Vector2 One()   noexcept { return Vector2{ 1, 1}; }
        static constexpr Vector2 Up()    noexcept { return Vector2{ 1, 0}; }
        static constexpr Vector2 Down()  noexcept { return Vector2{-1, 0}; }
        static constexpr Vector2 Left()  noexcept { return Vector2{ 0,-1}; }
        static constexpr Vector2 Right() noexcept { return Vector2{ 0, 1}; }
    };

    constexpr Vector2 operator +(const Vector2& lhs, const Vector2& rhs)
        { return Vector2(lhs.x + rhs.x, lhs.y + rhs.y); }

    constexpr Vector2 operator -(const Vector2& lhs, const Vector2& rhs)
        { return Vector2(lhs.x - rhs.x, lhs.y - rhs.y); }

    constexpr Vector2 operator *(const Vector2& vec, float scalar)
        { return Vector2(vec.x * scalar, vec.y * scalar); }

    constexpr Vector2 operator /(const Vector2& vec, float scalar)
        { return Vector2(vec.x / scalar, vec.y / scalar); }

    constexpr bool operator ==(const Vector2& lhs, const Vector2& rhs)
        { return math::FloatEqual(lhs.x, rhs.x) && math::FloatEqual(lhs.y, rhs.y); }

    constexpr bool operator !=(const Vector2& lhs, const Vector2& rhs)
        { return !(lhs == rhs); }


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

    constexpr Vector2 Lerp(const Vector2& lhs, const Vector2& rhs, float factor) noexcept
    { 
        factor = math::Clamp(factor, 0.0f, 1.0f);
        return Vector2{math::Lerp(lhs.x, rhs.x, factor), math::Lerp(lhs.y, rhs.y, factor)};
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