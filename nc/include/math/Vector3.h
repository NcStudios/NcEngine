#pragma once
#include "Math.h"
#include "Vector2.h"

namespace DirectX { struct XMFLOAT3; }

namespace nc
{
    struct Vector3
    {
        float x, y, z;

        constexpr explicit Vector3() noexcept : x{0.0f}, y{0.0f}, z{0.0f} {}
        constexpr explicit Vector3(float X, float Y, float Z) noexcept : x{X}, y{Y}, z{Z} {}
        constexpr explicit Vector3(float X, Vector2 YZ) noexcept : x{X}, y{YZ.x}, z{YZ.y} {}
        constexpr explicit Vector3(Vector2 XY, float Z) noexcept : x{XY.x}, y{XY.y}, z{Z} {}
        Vector3(const DirectX::XMFLOAT3& xm) noexcept;
        Vector3(DirectX::XMFLOAT3&& xm) noexcept;
        Vector3& operator=(const DirectX::XMFLOAT3& xm) noexcept;
        Vector3& operator=(DirectX::XMFLOAT3&& xm) noexcept;
        DirectX::XMFLOAT3 ToXMFloat3() const noexcept;

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

    constexpr Vector3 operator +(const Vector3& lhs, const Vector3& rhs)
        { return Vector3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z}; }

    constexpr Vector3 operator -(const Vector3& lhs, const Vector3& rhs)
        { return Vector3{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z}; }

    constexpr Vector3 operator *(const Vector3& vec, float scalar)
        { return Vector3{vec.x * scalar, vec.y * scalar, vec.z * scalar}; }
    
    constexpr Vector3 operator *(float scalar, const Vector3& vec)
        { return Vector3{vec.x * scalar, vec.y * scalar, vec.z * scalar}; }

    constexpr Vector3 operator /(const Vector3& vec, float scalar)
        { return Vector3{vec.x / scalar, vec.y / scalar, vec.z / scalar}; }

    constexpr bool operator ==(const Vector3& lhs, const Vector3& rhs)
        { return math::FloatEqual(lhs.x, rhs.x) && math::FloatEqual(lhs.y, rhs.y) && math::FloatEqual(lhs.z, rhs.z); }

    constexpr bool operator !=(const Vector3& lhs, const Vector3& rhs)
        { return !(lhs == rhs); }

    constexpr float SquareMagnitude(const Vector3& vec) noexcept
    {
        return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
    }
    
    constexpr float Magnitude(const Vector3& vec) noexcept
    {
        return sqrt(SquareMagnitude(vec));
    }

    constexpr Vector3 Normalize(const Vector3& vec) noexcept
    {
        return vec == Vector3::Zero() ? Vector3::Zero() : vec / Magnitude(vec);
    }
} // end namespace nc