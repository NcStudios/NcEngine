#pragma once
#include "Math.h"

namespace nc
{
    struct Vector4
    {
        float x;
        float y;
        float z;
        float w;

        Vector4() noexcept
            : x(0), y(0), z(0), w(0) {}

        Vector4(float x, float y, float z, float w) noexcept
            : x(x), y(y), z(z), w(w) {}

        Vector4(const Vector4& vec) noexcept
            : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}

        Vector4(Vector4&& vec) noexcept
            : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}

        Vector4& operator=(const Vector4& vec) noexcept
        {
            x = vec.x;  y = vec.y;
            z = vec.z;  w = vec.w;
            return *this;
        }

        Vector4& operator=(Vector4&& vec) noexcept
        {
            x = vec.x;  y = vec.y;
            z = vec.z;  w = vec.w;
            return *this;
        }

        inline void InvertX() noexcept { x *= -1.0f; }
        inline void InvertY() noexcept { y *= -1.0f; }
        inline void InvertZ() noexcept { z *= -1.0f; }
        inline void InvertW() noexcept { w *= -1.0f; }
        
        inline float Magnitude() const noexcept
            { return sqrt( (x * x) + (y * y) + (z * z) + (w * w) ); }

        inline float SquareMagnitude() const noexcept
            { return SquareMagnitude(*this); }
        
        inline Vector4 GetNormalized() const noexcept
        {
            float mag = Magnitude();
            return mag == 0 ? Zero() : Vector4(x / mag, y / mag, z / mag, w / mag);
        }

        inline void Normalize() noexcept
        {
            if(float mag = Magnitude(); mag != 0)
                { x /= mag, y /= mag, z /= mag, w /= mag; }
        }
        
        inline void TranslateBy(const Vector4& vec) noexcept
            { x += vec.x, y += vec.y, z += vec.z, w += vec.w; }

        static Vector4 Zero()  { return Vector4(0,0,0,0); }
        static Vector4 One()   { return Vector4(1,1,1,1); }

        static float SquareMagnitude(const Vector4& vec) noexcept
            { return ( (vec.x*vec.x) + (vec.y*vec.y) + (vec.z*vec.z) + (vec.w*vec.w) ); }
    };

    /*************
     * Operators *
     * ***********/
    inline Vector4 operator +(const Vector4& lhs, const Vector4& rhs)
        { return Vector4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }

    inline Vector4 operator -(const Vector4& lhs, const Vector4& rhs)
        { return Vector4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }

    inline Vector4 operator *(const Vector4& vec, const double scalar)
        { return Vector4(vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar); }

    inline Vector4 operator /(const Vector4& vec, const double scalar)
        { return Vector4(vec.x / scalar, vec.y / scalar, vec.z / scalar, vec.w / scalar); }

    inline bool operator ==(const Vector4& lhs, const Vector4& rhs)
        { return Vector4::SquareMagnitude(lhs - rhs) <= math::EPSILON; }

    inline bool operator !=(const Vector4& lhs, const Vector4& rhs)
        { return !(lhs == rhs); }

} // end namespace nc