#pragma once

#include "MathNCE.h"

namespace nc
{
    class Vector2
    {
        public:
            Vector2() noexcept
                : m_x(0), m_y(0) {}

            Vector2(float x, float y) noexcept 
                : m_x(x),       m_y(y)       {}

            Vector2(const Vector2& vec) noexcept 
                : m_x(vec.m_x), m_y(vec.m_y) {}

            Vector2(Vector2&& vec) noexcept 
                : m_x(vec.m_x), m_y(vec.m_y) {}

            Vector2& operator=(const Vector2& other) noexcept 
            {
                m_x = other.m_x; m_y = other.m_y;
                return *this;
            }
            
            Vector2& operator=(Vector2&& other) noexcept
            {
                m_x = other.m_x; m_y = other.m_y;
                return *this;
            }

            inline float X() const noexcept { return m_x; }
            inline float Y() const noexcept { return m_y; }

            inline void InvertX() noexcept { m_x *= -1.0f; }
            inline void InvertY() noexcept { m_y *= -1.0f; }

            inline float Magnitude() const noexcept
                { return sqrt( (m_x * m_x) + (m_y * m_y) ); }

            inline float   SquareMagnitude() const noexcept
                { return SquareMagnitude(*this); }

            inline Vector2 GetNormalized() const noexcept
            {
                    float mag = Magnitude();
                    return mag == 0 ? Zero() : Vector2(m_x / mag, m_y / mag);
            }

            //inline Vector3 ToVector3()       const noexcept;
            //inline Vector4 ToVector4()       const noexcept;

            inline void Normalize() noexcept
            { 
                if (float mag = Magnitude(); mag != 0) 
                    { m_x /= mag; m_y /= mag; }
            }

            inline void TranslateBy(const Vector2& vec) noexcept
            {
                m_x += vec.m_x, m_y += vec.m_y;
            }

            static Vector2 Zero()  { return Vector2( 0, 0); }
            static Vector2 One()   { return Vector2( 1, 1); }
            static Vector2 Up()    { return Vector2( 1, 0); }
            static Vector2 Down()  { return Vector2(-1, 0); }
            static Vector2 Left()  { return Vector2( 0,-1); }
            static Vector2 Right() { return Vector2( 0, 1); }
            
            static float SquareMagnitude(const Vector2& vec) noexcept
            {
                return   vec.m_x * vec.m_x 
                       + vec.m_y * vec.m_y;
            }

            static Vector2 Lerp(const Vector2& lhs, const Vector2& rhs, float factor) noexcept
            { 
                factor = math::Clamp(factor, 0, 1);
                return Vector2(lhs.m_x + (rhs.m_x - lhs.m_x) * factor,
                               lhs.m_y + (rhs.m_y - lhs.m_y) * factor);
            }

            static float Dot(const Vector2& lhs, const Vector2& rhs) noexcept
            {
                return   lhs.m_x * rhs.m_x 
                       + lhs.m_y * rhs.m_y;
            }

            static float Distance(const Vector2& lhs, const Vector2& rhs) noexcept
            {
                float x = lhs.m_x - rhs.m_x;
                float y = lhs.m_y - rhs.m_y;
                return sqrt(x * x + y * y);
            }

        private:
            float m_x, m_y;
    };

    /*************
     * Operators *
     *************/
    inline Vector2 operator +(const Vector2& lhs, const Vector2& rhs)
    { return Vector2(lhs.X() + rhs.X(), lhs.Y() + rhs.Y()); }

    inline Vector2 operator -(const Vector2& lhs, const Vector2& rhs)
        { return Vector2(lhs.X() - rhs.X(), lhs.Y() - rhs.Y()); }

    inline Vector2 operator *(const Vector2& vec, const double scalar)
        { return Vector2(vec.X() * scalar, vec.Y() * scalar); }

    inline Vector2 operator /(const Vector2& vec, const double scalar)
        { return Vector2(vec.X() / scalar, vec.Y() / scalar); }

    inline bool operator ==(const Vector2& lhs, const Vector2& rhs)
        { return Vector2::SquareMagnitude(lhs - rhs) <= math::EPSILON; }

    inline bool operator !=(const Vector2& lhs, const Vector2& rhs)
        { return !(lhs == rhs); }

} // end namespace nc