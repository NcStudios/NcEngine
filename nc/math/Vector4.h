#pragma once
#include "math/MathNCE.h"

namespace nc
{
    class Vector4
    {
        private:
            float m_x, m_y, m_z, m_w;

        public:
            Vector4() noexcept
                : m_x(0), m_y(0), m_z(0), m_w(0) {}

            Vector4(float x, float y, float z, float w) noexcept
                : m_x(x), m_y(y), m_z(z), m_w(w) {}

            Vector4(const Vector4& vec) noexcept
                : m_x(vec.m_x), m_y(vec.m_y), m_z(vec.m_z), m_w(vec.m_w) {}

            Vector4(Vector4&& vec) noexcept
                : m_x(vec.m_x), m_y(vec.m_y), m_z(vec.m_z), m_w(vec.m_w) {}

            Vector4& operator=(const Vector4& vec) noexcept
            {
                m_x = vec.m_x;  m_y = vec.m_y;
                m_z = vec.m_z;  m_w = vec.m_w;
                return *this;
            }

            Vector4& operator=(Vector4&& vec) noexcept
            {
                m_x = vec.m_x;  m_y = vec.m_y;
                m_z = vec.m_z;  m_w = vec.m_w;
                return *this;
            }

            inline float X() const noexcept { return m_x; }
            inline float Y() const noexcept { return m_y; }
            inline float Z() const noexcept { return m_z; }
            inline float W() const noexcept { return m_w; }

            inline void InvertX() noexcept { m_x *= -1.0f; }
            inline void InvertY() noexcept { m_y *= -1.0f; }
            inline void InvertZ() noexcept { m_z *= -1.0f; }
            inline void InvertW() noexcept { m_w *= -1.0f; }
            
            inline float Magnitude() const noexcept
                { return sqrt( (m_x * m_x) + (m_y * m_y) + (m_z * m_z) + (m_w * m_w) ); }

            inline float SquareMagnitude() const noexcept
                { return SquareMagnitude(*this); }
            
            inline Vector4 GetNormalized() const noexcept
            {
                float mag = Magnitude();
                return mag == 0 ? Zero() : Vector4(m_x / mag, m_y / mag, m_z / mag, m_w / mag);
            }

            //inline Vector2 ToVector2() const noexcept;
            //inline Vector3 ToVector3() const noexcept;

            inline void Normalize() noexcept
            {
                if(float mag = Magnitude(); mag != 0)
                    { m_x /= mag, m_y /= mag, m_z /= mag, m_w /= mag; }
            }

            //inline void TranslateBy(const Vector2& vec) noexcept;
            //inline void TranslateBy(const Vector3& vec) noexcept;
            
            inline void TranslateBy(const Vector4& vec) noexcept
                { m_x += vec.m_x, m_y += vec.m_y, m_z += vec.m_z, m_w += vec.m_w; }

            static Vector4 Zero()  { return Vector4(0,0,0,0); }
            static Vector4 One()   { return Vector4(1,1,1,1); }

            static float SquareMagnitude(const Vector4& vec) noexcept
                { return ( (vec.m_x*vec.m_x) + (vec.m_y*vec.m_y) + (vec.m_z*vec.m_z) + (vec.m_w*vec.m_w) ); }
    };

    /*************
     * Operators *
     * ***********/
    inline Vector4 operator +(const Vector4& lhs, const Vector4& rhs)
        { return Vector4(lhs.X() + rhs.X(), lhs.Y() + rhs.Y(), lhs.Z() + rhs.Z(), lhs.W() + rhs.W()); }

    inline Vector4 operator -(const Vector4& lhs, const Vector4& rhs)
        { return Vector4(lhs.X() - rhs.X(), lhs.Y() - rhs.Y(), lhs.Z() - rhs.Z(), lhs.W() - rhs.W()); }

    inline Vector4 operator *(const Vector4& vec, const double scalar)
        { return Vector4(vec.X() * scalar, vec.Y() * scalar, vec.Z() * scalar, vec.W() * scalar); }

    inline Vector4 operator /(const Vector4& vec, const double scalar)
        { return Vector4(vec.X() / scalar, vec.Y() / scalar, vec.Z() / scalar, vec.W() / scalar); }

    inline bool operator ==(const Vector4& lhs, const Vector4& rhs)
        { return Vector4::SquareMagnitude(lhs - rhs) <= math::EPSILON; }

    inline bool operator !=(const Vector4& lhs, const Vector4& rhs)
        { return !(lhs == rhs); }

} // end namespace nc