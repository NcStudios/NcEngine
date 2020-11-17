#pragma once
#include "Math.h"

namespace DirectX { struct XMFLOAT3; }

namespace nc
{
    struct Vector2;

    struct Vector3
    {
        float x;
        float y;
        float z;

        Vector3() noexcept;
        Vector3(float x, float y, float z) noexcept;
        Vector3(const Vector3& other) noexcept;
        Vector3(Vector3&& other) noexcept;
        Vector3(float x, Vector2 yz) noexcept;
        Vector3(Vector2 xy, float z) noexcept;
        Vector3& operator=(const Vector3& other) noexcept;
        Vector3& operator=(Vector3&& other) noexcept;

        Vector3(const DirectX::XMFLOAT3& xm) noexcept;
        Vector3(DirectX::XMFLOAT3&& xm) noexcept;
        Vector3& operator=(const DirectX::XMFLOAT3& xm) noexcept;
        Vector3& operator=(DirectX::XMFLOAT3&& xm) noexcept;
        DirectX::XMFLOAT3 ToXMFloat3() const noexcept;

        inline void InvertX() noexcept { x *= -1.0f; }
        inline void InvertY() noexcept { y *= -1.0f; }
        inline void InvertZ() noexcept { z *= -1.0f; }

        inline float Magnitude() const noexcept
            { return sqrt( (x * x) + (y * y) + (z * z) ); }

        inline float SquareMagnitude() const noexcept
            { return SquareMagnitude(*this); }

        inline Vector3 GetNormalized() const noexcept
        {
            float mag = Magnitude();
            return mag == 0 ? Zero() : Vector3(x / mag, y / mag, z / mag);
        }

        inline void Normalize() noexcept
        {
            if(float mag = Magnitude(); mag != 0)
                { x /= mag, y /= mag, z /= mag; }
        }

        inline void TranslateBy(const Vector3& vec) noexcept
            { x += vec.x, y += vec.y, z += vec.z; }

        static Vector3 Zero()  noexcept { return Vector3( 0, 0, 0); }
        static Vector3 One()   noexcept { return Vector3( 1, 1, 1); }
        static Vector3 Up()    noexcept { return Vector3( 0, 1, 0); }
        static Vector3 Down()  noexcept { return Vector3( 0,-1, 0); }
        static Vector3 Left()  noexcept { return Vector3(-1, 0, 0); }
        static Vector3 Right() noexcept { return Vector3( 1, 0, 0); }
        static Vector3 Front() noexcept { return Vector3( 0, 0, 1); }
        static Vector3 Back()  noexcept { return Vector3( 0, 0,-1); }

        static float SquareMagnitude(const Vector3& vec) noexcept
        {
            return ( (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z) );
        }
    };

    /*************
     * Operators *
     * ***********/
    inline Vector3 operator +(const Vector3& lhs, const Vector3& rhs)
        { return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z); }

    inline Vector3 operator -(const Vector3& lhs, const Vector3& rhs)
        { return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z); }

    inline Vector3 operator *(const Vector3& vec, const double scalar)
        { return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar); }
    
    inline Vector3 operator *(const double scalar, const Vector3& vec)
        { return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar); }

    inline Vector3 operator /(const Vector3& vec, const double scalar)
        { return Vector3(vec.x / scalar, vec.y / scalar, vec.z / scalar); }

    inline bool operator ==(const Vector3& lhs, const Vector3& rhs)
        { return Vector3::SquareMagnitude(lhs - rhs) <= math::EPSILON; }

    inline bool operator !=(const Vector3& lhs, const Vector3& rhs)
        { return !(lhs == rhs); }

} // end namespace nc