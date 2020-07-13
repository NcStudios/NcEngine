#pragma once
#include "Math.h"

namespace nc      { class  Transform; }
namespace DirectX { struct XMFLOAT3; }

namespace nc
{
    class Vector2;

    class Vector3
    {
        friend nc::Transform;

        public:
            Vector3()                                       noexcept;
            Vector3(float x, float y, float z)              noexcept;
            Vector3(const Vector3& other)                   noexcept;
            Vector3(Vector3&& other)                        noexcept;
            Vector3(const DirectX::XMFLOAT3& xm)            noexcept;
            Vector3(DirectX::XMFLOAT3&& xm)                 noexcept;
            Vector3(float x, Vector2 yz)                    noexcept;
            Vector3(Vector2 xy, float z)                    noexcept;
            Vector3& operator=(const Vector3& other)        noexcept;
            Vector3& operator=(const DirectX::XMFLOAT3& xm) noexcept;
            Vector3& operator=(Vector3&& other)             noexcept;
            Vector3& operator=(DirectX::XMFLOAT3&& xm)      noexcept;

            DirectX::XMFLOAT3 GetXMFloat3() const noexcept;

            inline float X() const noexcept { return m_x; }
            inline float Y() const noexcept { return m_y; }
            inline float Z() const noexcept { return m_z; }

            inline void InvertX() noexcept { m_x *= -1.0f; }
            inline void InvertY() noexcept { m_y *= -1.0f; }
            inline void InvertZ() noexcept { m_z *= -1.0f; }

            inline float Magnitude() const noexcept
                { return sqrt( (m_x * m_x) + (m_y * m_y) + (m_z * m_z) ); }

            inline float SquareMagnitude() const noexcept
                { return SquareMagnitude(*this); }

            inline Vector3 GetNormalized() const noexcept
            {
                float mag = Magnitude();
                return mag == 0 ? Zero() : Vector3(m_x / mag, m_y / mag, m_z / mag);
            }

            //inline Vector2 ToVector2()       const noexcept;
            //inline Vector4 ToVector4()       const noexcept;

            inline void Normalize() noexcept
            {
                if(float mag = Magnitude(); mag != 0)
                    { m_x /= mag, m_y /= mag, m_z /= mag; }
            }

            //inline void TranslateBy(const Vector2& vec) noexcept;

            inline void TranslateBy(const Vector3& vec) noexcept
                { m_x += vec.m_x, m_y += vec.m_y, m_z += vec.m_z; }

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
                return ( (vec.m_x * vec.m_x) + (vec.m_y * vec.m_y) + (vec.m_z * vec.m_z) );
            }

        private:
            float m_x, m_y, m_z;
    };

    /*************
     * Operators *
     * ***********/
    inline Vector3 operator +(const Vector3& lhs, const Vector3& rhs)
        { return Vector3(lhs.X() + rhs.X(), lhs.Y() + rhs.Y(), lhs.Z() + rhs.Z()); }

    inline Vector3 operator -(const Vector3& lhs, const Vector3& rhs)
        { return Vector3(lhs.X() - rhs.X(), lhs.Y() - rhs.Y(), lhs.Z() - rhs.Z()); }

    inline Vector3 operator *(const Vector3& vec, const double scalar)
        { return Vector3(vec.X() * scalar, vec.Y() * scalar, vec.Z() * scalar); }
    
    inline Vector3 operator *(const double scalar, const Vector3& vec)
        { return Vector3(vec.X() * scalar, vec.Y() * scalar, vec.Z() * scalar); }

    inline Vector3 operator /(const Vector3& vec, const double scalar)
        { return Vector3(vec.X() / scalar, vec.Y() / scalar, vec.Z() / scalar); }

    inline bool operator ==(const Vector3& lhs, const Vector3& rhs)
        { return Vector3::SquareMagnitude(lhs - rhs) <= math::EPSILON; }

    inline bool operator !=(const Vector3& lhs, const Vector3& rhs)
        { return !(lhs == rhs); }

} // end namespace nc