#pragma once
#include "Math.h"

struct ImVec2;

namespace nc
{
    struct Vector2
    {
        float x;
        float y;

        constexpr Vector2() noexcept
            : x{0.0f}, y{0.0f} {}

        constexpr Vector2(float X, float Y) noexcept 
            : x{X}, y{Y} {}

        Vector2(const ImVec2& vec) noexcept;
        Vector2(ImVec2&& vec) noexcept;
        Vector2& operator=(const ImVec2& vec) noexcept;
        Vector2& operator=(ImVec2&& vec) noexcept;
        ImVec2 ToImVec2() noexcept;

        inline void InvertX() noexcept { x *= -1.0f; }
        inline void InvertY() noexcept { y *= -1.0f; }

        inline float Magnitude() const noexcept
        {
            return sqrt( (x * x) + (y * y) );
        }

        inline float   SquareMagnitude() const noexcept
        {
            return SquareMagnitude(*this);
        }

        inline Vector2 GetNormalized() const noexcept
        {
            float mag = Magnitude();
            return mag == 0 ? Zero() : Vector2(x / mag, y / mag);
        }

        inline void Normalize() noexcept
        { 
            if (float mag = Magnitude(); mag != 0) 
            {
                x /= mag;
                y /= mag;
            }
        }

        inline void TranslateBy(const Vector2& vec) noexcept
        {
            x += vec.x;
            y += vec.y;
        }

        static Vector2 Zero()  { return Vector2{ 0, 0}; }
        static Vector2 One()   { return Vector2{ 1, 1}; }
        static Vector2 Up()    { return Vector2{ 1, 0}; }
        static Vector2 Down()  { return Vector2{-1, 0}; }
        static Vector2 Left()  { return Vector2{ 0,-1}; }
        static Vector2 Right() { return Vector2{ 0, 1}; }
        
        static float SquareMagnitude(const Vector2& vec) noexcept
        {
            return   vec.x * vec.x 
                    + vec.y * vec.y;
        }

        static Vector2 Lerp(const Vector2& lhs, const Vector2& rhs, float factor) noexcept
        { 
            factor = math::Clamp(factor, 0.0f, 1.0f);
            return Vector2{lhs.x + (rhs.x - lhs.x) * factor,
                            lhs.y + (rhs.y - lhs.y) * factor};
        }

        static float Dot(const Vector2& lhs, const Vector2& rhs) noexcept
        {
            return lhs.x * rhs.x + lhs.y * rhs.y;
        }

        static float Distance(const Vector2& lhs, const Vector2& rhs) noexcept
        {
            float x = lhs.x - rhs.x;
            float y = lhs.y - rhs.y;
            return sqrt(x * x + y * y);
        }
    };

    /*************
     * Operators *
     *************/
    inline Vector2 operator +(const Vector2& lhs, const Vector2& rhs)
    { return Vector2(lhs.x + rhs.x, lhs.y + rhs.y); }

    inline Vector2 operator -(const Vector2& lhs, const Vector2& rhs)
        { return Vector2(lhs.x - rhs.x, lhs.y - rhs.y); }

    inline Vector2 operator *(const Vector2& vec, float scalar)
        { return Vector2(vec.x * scalar, vec.y * scalar); }

    inline Vector2 operator /(const Vector2& vec, float scalar)
        { return Vector2(vec.x / scalar, vec.y / scalar); }

    inline bool operator ==(const Vector2& lhs, const Vector2& rhs)
        { return math::FloatEqual(lhs.x, rhs.x) && math::FloatEqual(lhs.y, rhs.y); }

    inline bool operator !=(const Vector2& lhs, const Vector2& rhs)
        { return !(lhs == rhs); }

} // end namespace nc