#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>  //sqrt
#include <limits> //numeric_limits::epsilon
#include "MathNCE.h"

namespace nc
{

const double EPSILON = std::numeric_limits<float>::epsilon();

class Vector3;
class Vector4;

class Vector2
{
    public:
        Vector2() noexcept : m_x(0), m_y(0) {}
        Vector2(float x, float y) noexcept : m_x(x), m_y(y) {}

        static Vector2 Zero()  { return Vector2( 0,  0); }
        static Vector2 One()   { return Vector2( 1,  1); }
        static Vector2 Up()    { return Vector2( 1,  0); }
        static Vector2 Down()  { return Vector2(-1,  0); }
        static Vector2 Left()  { return Vector2( 0, -1); }
        static Vector2 Right() { return Vector2( 0,  1); }
        static float SquareMagnitude(const Vector2& vec) { return ( (vec.m_x * vec.m_x) + (vec.m_y * vec.m_y) ); }

        inline float GetX() const noexcept { return m_x; }
        inline float GetY() const noexcept { return m_y; }

        float Magnitude() const noexcept;
        float SquareMagnitude() const noexcept;
        Vector2 GetNormalized() const noexcept;
        Vector3 ToVector3() const noexcept;
        Vector4 ToVector4() const noexcept;

        void Normalize() noexcept;
        void TranslateBy(const Vector2& vec) noexcept;

        //new - need to add to 3 and 4
        static Vector2 Lerp(const Vector2& lhs, const Vector2& rhs, float factor) noexcept
        { 
            factor = math::MathNCE::Clamp(factor, 0, 1);
            return Vector2(lhs.m_x + (rhs.m_x - lhs.m_x) * factor,
                           lhs.m_y + (rhs.m_y - lhs.m_y) * factor);
        }

        static float Dot(const Vector2& lhs, const Vector2& rhs) noexcept
        {
            return lhs.m_x * rhs.m_x + lhs.m_y * rhs.m_y;
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

class Vector3
{
    public:
        Vector3() noexcept : m_x(0), m_y(0), m_z(0) {}
        Vector3(float x, float y, float z) noexcept : m_x(x), m_y(y), m_z(z) {}

        static Vector3 Zero()  { return Vector3( 0,  0,  0); }
        static Vector3 One()   { return Vector3( 1,  1,  1); }
        static Vector3 Up()    { return Vector3( 0,  1,  0); }
        static Vector3 Down()  { return Vector3( 0, -1,  0); }
        static Vector3 Left()  { return Vector3(-1,  0,  0); }
        static Vector3 Right() { return Vector3( 1,  0,  0); }
        static Vector3 Front() { return Vector3( 0,  0,  1); }
        static Vector3 Back()  { return Vector3( 0,  0, -1); }
        static float SquareMagnitude(const Vector3& vec) { return ( (vec.m_x * vec.m_x) + (vec.m_y * vec.m_y) + (vec.m_z * vec.m_z) ); }

        inline float GetX() const noexcept { return m_x; }
        inline float GetY() const noexcept { return m_y; }
        inline float GetZ() const noexcept { return m_z; }

        float Magnitude() const noexcept;
        float SquareMagnitude() const noexcept;
        Vector3 GetNormalized() const noexcept;
        Vector2 ToVector2() const noexcept;
        Vector4 ToVector4() const noexcept;

        void Normalize() noexcept;
        void TranslateBy(const Vector2& vec) noexcept;
        void TranslateBy(const Vector3& vec) noexcept;
    
    private:
        float m_x, m_y, m_z;
};

class Vector4
{
    public:
        Vector4() noexcept : m_x(0), m_y(0), m_z(0), m_w(0) {}
        Vector4(float x, float y, float z, float w) noexcept : m_x(x), m_y(y), m_z(z), m_w(w) {}

        static Vector4 Zero()  { return Vector4( 0, 0, 0, 0); }
        static Vector4 One()   { return Vector4( 1, 1, 1, 1); }
        static float SquareMagnitude(const Vector4& vec) { return ( (vec.m_x*vec.m_x) + (vec.m_y*vec.m_y) + (vec.m_z*vec.m_z) + (vec.m_w*vec.m_w) ); }

        inline float GetX() const noexcept { return m_x; }
        inline float GetY() const noexcept { return m_y; }
        inline float GetZ() const noexcept { return m_z; }
        inline float GetW() const noexcept { return m_w; }

        float Magnitude() const noexcept;
        float SquareMagnitude() const noexcept;
        Vector4 GetNormalized() const noexcept;
        Vector2 ToVector2() const noexcept;
        Vector3 ToVector3() const noexcept;

        void Normalize() noexcept;
        void TranslateBy(const Vector2& vec) noexcept;
        void TranslateBy(const Vector3& vec) noexcept;
        void TranslateBy(const Vector4& vec) noexcept;
    
    private:
        float m_x, m_y, m_z, m_w;
};

Vector2 operator +(const Vector2& lhs, const Vector2& rhs);
Vector2 operator -(const Vector2& lhs, const Vector2& rhs);
Vector2 operator *(const Vector2& vec, const double scalar);
Vector2 operator /(const Vector2& vec, const double scalar);
bool operator ==(const Vector2& lhs, const Vector2& rhs);
bool operator !=(const Vector2& lhs, const Vector2& rhs);

Vector3 operator +(const Vector3& lhs, const Vector3& rhs);
Vector3 operator -(const Vector3& lhs, const Vector3& rhs);
Vector3 operator *(const Vector3& vec, const double scalar);
Vector3 operator /(const Vector3& vec, const double scalar);
bool operator ==(const Vector3& lhs, const Vector3& rhs);
bool operator !=(const Vector3& lhs, const Vector3& rhs);

Vector4 operator +(const Vector4& lhs, const Vector4& rhs);
Vector4 operator -(const Vector4& lhs, const Vector4& rhs);
Vector4 operator *(const Vector4& vec, const double scalar);
Vector4 operator /(const Vector4& vec, const double scalar);
bool operator ==(const Vector4& lhs, const Vector4& rhs);
bool operator !=(const Vector4& lhs, const Vector4& rhs);

} //end namespace nc

#endif