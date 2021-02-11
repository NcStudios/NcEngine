#pragma once
#include "Math.h"

namespace nc
{
    struct Vector4
    {
        float x, y, z, w;

        constexpr explicit Vector4() noexcept : x{0.0f}, y{0.0f}, z{0.0f}, w{0.0f} {}
        constexpr explicit Vector4(float x, float y, float z, float w) noexcept : x{x}, y{y}, z{z}, w{w} {}

        static constexpr Vector4 Splat(float v) noexcept { return Vector4{v, v, v, v}; }
        static constexpr Vector4 Zero() noexcept { return Vector4(0, 0, 0, 0); }
        static constexpr Vector4 One() noexcept { return Vector4(1, 1, 1, 1); }
    };

    constexpr bool operator ==(const Vector4& lhs, const Vector4& rhs) noexcept;
    constexpr bool operator !=(const Vector4& lhs, const Vector4& rhs) noexcept;
    constexpr Vector4 operator +(const Vector4& lhs, const Vector4& rhs) noexcept;
    constexpr Vector4 operator -(const Vector4& lhs, const Vector4& rhs) noexcept;
    constexpr Vector4 operator *(const Vector4& vec, float scalar) noexcept;
    constexpr Vector4 operator *(float scalar, const Vector4& vec) noexcept;
    constexpr Vector4 operator /(const Vector4& vec, float scalar) noexcept;
    constexpr Vector4 HadamardProduct(const Vector4& lhs, const Vector4& rhs) noexcept;
    constexpr float SquareMagnitude(const Vector4& vec) noexcept;
    constexpr float Magnitude(const Vector4& vec) noexcept;
    constexpr Vector4 Normalize(const Vector4& vec) noexcept;
    constexpr Vector4 Lerp(const Vector4& lhs, const Vector4& rhs, float factor) noexcept;
    constexpr float Dot(const Vector4& lhs, const Vector4& rhs) noexcept;
    constexpr float Distance(const Vector4& lhs, const Vector4& rhs) noexcept;

    constexpr bool operator ==(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return math::FloatEqual(lhs.x, rhs.x) &&
               math::FloatEqual(lhs.y, rhs.y) &&
               math::FloatEqual(lhs.z, rhs.z) &&
               math::FloatEqual(lhs.w, rhs.w);
    }

    constexpr bool operator !=(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr Vector4 operator +(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return Vector4{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w};
    }

    constexpr Vector4 operator -(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return Vector4{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w};
    }

    constexpr Vector4 operator *(const Vector4& vec, float scalar) noexcept
    {
        return Vector4{vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar};
    }

    constexpr Vector4 operator *(float scalar, const Vector4& vec) noexcept
    {
        return vec * scalar;
    }

    constexpr Vector4 operator /(const Vector4& vec, float scalar) noexcept
    {
        return Vector4{vec.x / scalar, vec.y / scalar, vec.z / scalar, vec.w / scalar};
    }

    constexpr Vector4 HadamardProduct(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return Vector4{lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w};
    }

    constexpr float SquareMagnitude(const Vector4& vec) noexcept
    {
        return vec.x*vec.x + vec.y*vec.y + vec.z*vec.z + vec.w*vec.w;
    }

    constexpr float Magnitude(const Vector4& vec) noexcept
    {
        return sqrt(SquareMagnitude(vec));
    }

    constexpr Vector4 Normalize(const Vector4& vec) noexcept
    {
        return vec == Vector4::Zero() ? Vector4::Zero() : vec / Magnitude(vec);
    }

    constexpr Vector4 Lerp(const Vector4& lhs, const Vector4& rhs, float factor) noexcept
    {
        return math::Lerp(lhs, rhs, factor);
    }

    constexpr float Dot(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z + rhs.z + lhs.w + rhs.w;
    }

    constexpr float Distance(const Vector4& lhs, const Vector4& rhs) noexcept
    {
        float x = lhs.x - rhs.x;
        float y = lhs.y - rhs.y;
        float z = lhs.z - rhs.z;
        float w = lhs.w - rhs.w;
        return std::sqrt(x * x + y * y + z * z + w * w);
    }
} // end namespace nc