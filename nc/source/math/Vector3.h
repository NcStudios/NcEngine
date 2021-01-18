#pragma once
#include "Math.h"
#include "Vector2.h"

namespace DirectX { struct XMFLOAT3; }

namespace nc
{
    struct Vector3
    {
        float x, y, z;

        constexpr Vector3() noexcept
            : x{0.0f}, y{0.0f}, z{0.0f} {}

        constexpr Vector3(float X, float Y, float Z) noexcept
            : x{X}, y{Y}, z{Z} {}

        constexpr Vector3(float X, Vector2 YZ) noexcept
            : x{X}, y{YZ.x}, z{YZ.y} {}

        constexpr Vector3(Vector2 XY, float Z) noexcept
            : x{XY.x}, y{XY.y}, z{Z} {}

        Vector3(const DirectX::XMFLOAT3& xm) noexcept;
        Vector3(DirectX::XMFLOAT3&& xm) noexcept;
        Vector3& operator=(const DirectX::XMFLOAT3& xm) noexcept;
        Vector3& operator=(DirectX::XMFLOAT3&& xm) noexcept;
        DirectX::XMFLOAT3 ToXMFloat3() const noexcept;

        static Vector3 Zero()  noexcept { return Vector3{ 0, 0, 0}; }
        static Vector3 One()   noexcept { return Vector3{ 1, 1, 1}; }
        static Vector3 Up()    noexcept { return Vector3{ 0, 1, 0}; }
        static Vector3 Down()  noexcept { return Vector3{ 0,-1, 0}; }
        static Vector3 Left()  noexcept { return Vector3{-1, 0, 0}; }
        static Vector3 Right() noexcept { return Vector3{ 1, 0, 0}; }
        static Vector3 Front() noexcept { return Vector3{ 0, 0, 1}; }
        static Vector3 Back()  noexcept { return Vector3{ 0, 0,-1}; }

        static float SquareMagnitude(const Vector3& vec) noexcept
        {
            return (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z);
        }

        inline void InvertX() noexcept { x *= -1.0f; }
        inline void InvertY() noexcept { y *= -1.0f; }
        inline void InvertZ() noexcept { z *= -1.0f; }

        inline float Magnitude() const noexcept
        {
            return sqrt( (x * x) + (y * y) + (z * z) );
        }

        inline float SquareMagnitude() const noexcept
        {
            return SquareMagnitude(*this);
        }

        inline Vector3 GetNormalized() const noexcept
        {
            float mag = Magnitude();
            return mag == 0 ? Zero() : Vector3(x / mag, y / mag, z / mag);
        }

        inline void Normalize() noexcept
        {
            if(float mag = Magnitude(); mag != 0)
            {
                x /= mag;
                y /= mag;
                z /= mag;
            }
        }

        inline void TranslateBy(const Vector3& vec) noexcept
        {
            x += vec.x;
            y += vec.y;
            z += vec.z;
        }
    };

    /*************
     * Operators *
     * ***********/
    inline Vector3 operator +(const Vector3& lhs, const Vector3& rhs)
        { return Vector3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z}; }

    inline Vector3 operator -(const Vector3& lhs, const Vector3& rhs)
        { return Vector3{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z}; }

    inline Vector3 operator *(const Vector3& vec, float scalar)
        { return Vector3{vec.x * scalar, vec.y * scalar, vec.z * scalar}; }
    
    inline Vector3 operator *(float scalar, const Vector3& vec)
        { return Vector3{vec.x * scalar, vec.y * scalar, vec.z * scalar}; }

    inline Vector3 operator /(const Vector3& vec, float scalar)
        { return Vector3{vec.x / scalar, vec.y / scalar, vec.z / scalar}; }

    inline bool operator ==(const Vector3& lhs, const Vector3& rhs)
        { return Vector3::SquareMagnitude(lhs - rhs) <= math::EPSILON; }

    inline bool operator !=(const Vector3& lhs, const Vector3& rhs)
        { return !(lhs == rhs); }

} // end namespace nc