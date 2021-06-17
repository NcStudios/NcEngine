#pragma once
#include "Math.h"

namespace nc
{
    struct Vector3
    {
        float x, y, z;

        constexpr Vector3() noexcept : x{0.0f}, y{0.0f}, z{0.0f} {}
        constexpr explicit Vector3(float X, float Y, float Z) noexcept : x{X}, y{Y}, z{Z} {}

        static constexpr Vector3 Splat(float v) noexcept { return Vector3{v, v, v}; }
        static constexpr Vector3 Zero()  noexcept { return Vector3{ 0, 0, 0}; }
        static constexpr Vector3 One()   noexcept { return Vector3{ 1, 1, 1}; }
        static constexpr Vector3 Up()    noexcept { return Vector3{ 0, 1, 0}; }
        static constexpr Vector3 Down()  noexcept { return Vector3{ 0,-1, 0}; }
        static constexpr Vector3 Left()  noexcept { return Vector3{-1, 0, 0}; }
        static constexpr Vector3 Right() noexcept { return Vector3{ 1, 0, 0}; }
        static constexpr Vector3 Front() noexcept { return Vector3{ 0, 0, 1}; }
        static constexpr Vector3 Back()  noexcept { return Vector3{ 0, 0,-1}; }
    };

    constexpr bool operator ==(const Vector3& lhs, const Vector3& rhs) noexcept;
    constexpr bool operator !=(const Vector3& lhs, const Vector3& rhs) noexcept;
    constexpr Vector3 operator +(const Vector3& lhs, const Vector3& rhs) noexcept;
    constexpr Vector3 operator -(const Vector3& lhs, const Vector3& rhs) noexcept;
    constexpr Vector3 operator *(const Vector3& vec, float scalar) noexcept;
    constexpr Vector3 operator *(float scalar, const Vector3& vec) noexcept;
    constexpr Vector3 operator /(const Vector3& vec, float scalar) noexcept;
    constexpr Vector3 operator -(const Vector3& vec) noexcept;
    constexpr float Dot(const Vector3& lhs, const Vector3& rhs) noexcept;
    constexpr Vector3 CrossProduct(const Vector3& lhs, const Vector3& rhs) noexcept;
    constexpr Vector3 TripleCrossProduct(const Vector3& a, const Vector3& b, const Vector3& c) noexcept;
    constexpr Vector3 HadamardProduct(const Vector3& lhs, const Vector3& rhs) noexcept;
    constexpr float SquareMagnitude(const Vector3& vec) noexcept;
    constexpr float Magnitude(const Vector3& vec) noexcept;
    constexpr Vector3 Normalize(const Vector3& vec) noexcept;
    constexpr Vector3 Lerp(const Vector3& lhs, const Vector3& rhs, float factor) noexcept;
    constexpr float Distance(const Vector3& lhs, const Vector3& rhs) noexcept;
    constexpr bool HasAnyZeroElement(const Vector3& vec) noexcept;

    constexpr bool operator ==(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return math::FloatEqual(lhs.x, rhs.x) && math::FloatEqual(lhs.y, rhs.y) && math::FloatEqual(lhs.z, rhs.z);
    }

    constexpr bool operator !=(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr Vector3 operator +(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return Vector3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
    }

    constexpr Vector3 operator -(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return Vector3{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
    }

    constexpr Vector3 operator *(const Vector3& vec, float scalar) noexcept
    {
        return Vector3{vec.x * scalar, vec.y * scalar, vec.z * scalar};
    }
    
    constexpr Vector3 operator *(float scalar, const Vector3& vec) noexcept
    {
        return vec * scalar;
    }

    constexpr Vector3 operator /(const Vector3& vec, float scalar) noexcept
    {
        return Vector3{vec.x / scalar, vec.y / scalar, vec.z / scalar};
    }

    constexpr Vector3 operator-(const Vector3& vec) noexcept
    {
        return vec * -1.0f;
    }

    constexpr float Dot(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }
    
    constexpr Vector3 CrossProduct(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return Vector3
        {
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x
        };
    }

    constexpr Vector3 TripleCrossProduct(const Vector3& a, const Vector3& b, const Vector3& c) noexcept
    {
        return (b * Dot(c, a)) - (a * Dot(c, b));
    }

    constexpr Vector3 HadamardProduct(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        return Vector3{lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
    }

    constexpr float SquareMagnitude(const Vector3& vec) noexcept
    {
        return Dot(vec, vec);
    }
    
    constexpr float Magnitude(const Vector3& vec) noexcept
    {
        return sqrt(SquareMagnitude(vec));
    }

    constexpr Vector3 Normalize(const Vector3& vec) noexcept
    {
        return vec == Vector3::Zero() ? Vector3::Zero() : vec / Magnitude(vec);
    }

    constexpr Vector3 Lerp(const Vector3& lhs, const Vector3& rhs, float factor) noexcept
    {
        return math::Lerp(lhs, rhs, factor);
    }

    constexpr float Distance(const Vector3& lhs, const Vector3& rhs) noexcept
    {
        float x = lhs.x - rhs.x;
        float y = lhs.y - rhs.y;
        float z = lhs.z - rhs.z;
        return std::sqrt(x * x + y * y + z * z);
    }

    constexpr bool HasAnyZeroElement(const Vector3& vec) noexcept
    {
        return math::FloatEqual(vec.x, 0.0f) || math::FloatEqual(vec.y, 0.0f) || math::FloatEqual(vec.z, 0.0f);
    }
} // namespace nc