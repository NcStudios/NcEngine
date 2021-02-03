#pragma once
#include "Math.h"

namespace DirectX { struct XMFLOAT4; }

namespace nc
{
    struct Vector4
    {
        float x, y, z, w;

        constexpr explicit Vector4() noexcept : x{0.0f}, y{0.0f}, z{0.0f}, w{0.0f} {}
        constexpr explicit Vector4(float x, float y, float z, float w) noexcept : x{x}, y{y}, z{z}, w{w} {}
        Vector4(const DirectX::XMFLOAT4& xm) noexcept;
        Vector4(DirectX::XMFLOAT4&& xm) noexcept;
        DirectX::XMFLOAT4 ToXMFloat4() const noexcept;

        static constexpr Vector4 Splat(float v) noexcept { return Vector4{v, v, v, v}; }
        static constexpr Vector4 Zero() noexcept { return Vector4(0, 0, 0, 0); }
        static constexpr Vector4 One() noexcept { return Vector4(1, 1, 1, 1); }
    };

    constexpr Vector4 operator +(const Vector4& lhs, const Vector4& rhs)
        { return Vector4{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w}; }

    constexpr Vector4 operator -(const Vector4& lhs, const Vector4& rhs)
        { return Vector4{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w}; }

    constexpr Vector4 operator *(const Vector4& vec, float scalar)
        { return Vector4{vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar}; }

    constexpr Vector4 operator /(const Vector4& vec, float scalar)
        { return Vector4{vec.x / scalar, vec.y / scalar, vec.z / scalar, vec.w / scalar}; }

    constexpr bool operator ==(const Vector4& lhs, const Vector4& rhs)
    {
        return math::FloatEqual(lhs.x, rhs.x) &&
               math::FloatEqual(lhs.y, rhs.y) &&
               math::FloatEqual(lhs.z, rhs.z) &&
               math::FloatEqual(lhs.w, rhs.w);
    }

    constexpr bool operator !=(const Vector4& lhs, const Vector4& rhs)
        { return !(lhs == rhs); }

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
} // end namespace nc