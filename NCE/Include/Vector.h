#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>  //sqrt
#include <limits> //numeric_limits::epsilon

#include "MathNCE.h"

namespace nc
{

const double EPSILON = std::numeric_limits<double>::epsilon();

class Vector3;
class Vector4;

class Vector2
{
    private:
        double m_x, m_y;

    public:
        Vector2() : m_x(0), m_y(0) {}
        Vector2(double x, double y) : m_x(x), m_y(y) {}

        static Vector2 Zero()  { return Vector2( 0,  0); }
        static Vector2 One()   { return Vector2( 1,  1); }
        static Vector2 Up()    { return Vector2( 1,  0); }
        static Vector2 Down()  { return Vector2(-1,  0); }
        static Vector2 Left()  { return Vector2( 0, -1); }
        static Vector2 Right() { return Vector2( 0,  1); }
        static double SquareMagnitude(const Vector2& vec) { return ( (vec.m_x * vec.m_x) + (vec.m_y * vec.m_y) ); }

        double GetX() const { return m_x; }
        double GetY() const { return m_y; }

        double Magnitude() const;
        double SquareMagnitude() const;
        Vector2 GetNormalized() const;
        Vector3 ToVector3() const;
        Vector4 ToVector4() const;

        void Normalize();
        void TranslateBy(const Vector2& vec);

        //new - need to add to 3 and 4
        static Vector2 Lerp(const Vector2& lhs, const Vector2& rhs, float factor)
        { 
            factor = math::MathNCE::Clamp(factor, 0, 1);
            return Vector2(lhs.m_x + (rhs.m_x - lhs.m_x) * factor,
                           lhs.m_y + (rhs.m_y - lhs.m_y) * factor);
        }

        static float Dot(const Vector2& lhs, const Vector2& rhs)
        {
            return lhs.m_x * rhs.m_x + lhs.m_y * rhs.m_y;
        }

        static float Distance(const Vector2& lhs, const Vector2& rhs)
        {
            float x = lhs.m_x - rhs.m_x;
            float y = lhs.m_y - rhs.m_y;
            return sqrt(x * x + y * y);
        }
};

class Vector3
{
    private:
        double m_x, m_y, m_z;

    public:
        Vector3() : m_x(0), m_y(0), m_z(0) {}
        Vector3(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}

        static Vector3 Zero()  { return Vector3( 0,  0,  0); }
        static Vector3 One()   { return Vector3( 1,  1,  1); }
        static Vector3 Up()    { return Vector3( 0,  1,  0); }
        static Vector3 Down()  { return Vector3( 0, -1,  0); }
        static Vector3 Left()  { return Vector3(-1,  0,  0); }
        static Vector3 Right() { return Vector3( 1,  0,  0); }
        static Vector3 Front() { return Vector3( 0,  0,  1); }
        static Vector3 Back()  { return Vector3( 0,  0, -1); }
        static double SquareMagnitude(const Vector3& vec) { return ( (vec.m_x * vec.m_x) + (vec.m_y * vec.m_y) + (vec.m_z * vec.m_z) ); }

        double GetX() const { return m_x; }
        double GetY() const { return m_y; }
        double GetZ() const { return m_z; }

        double Magnitude() const;
        double SquareMagnitude() const;
        Vector3 GetNormalized() const;
        Vector2 ToVector2() const;
        Vector4 ToVector4() const;

        void Normalize();
        void TranslateBy(const Vector2& vec);
        void TranslateBy(const Vector3& vec);
};

class Vector4
{
    private:
        double m_x, m_y, m_z, m_w;

    public:
        Vector4() : m_x(0), m_y(0), m_z(0), m_w(0) {}
        Vector4(double x, double y, double z, double w) : m_x(x), m_y(y), m_z(z), m_w(w) {}

        static Vector4 Zero()  { return Vector4( 0, 0, 0, 0); }
        static Vector4 One()   { return Vector4( 1, 1, 1, 1); }
        static double SquareMagnitude(const Vector4& vec) { return ( (vec.m_x*vec.m_x) + (vec.m_y*vec.m_y) + (vec.m_z*vec.m_z) + (vec.m_w*vec.m_w) ); }

        double GetX() const { return m_x; }
        double GetY() const { return m_y; }
        double GetZ() const { return m_z; }
        double GetW() const { return m_w; }

        double Magnitude() const;
        double SquareMagnitude() const;
        Vector4 GetNormalized() const;
        Vector2 ToVector2() const;
        Vector3 ToVector3() const;

        void Normalize();
        void TranslateBy(const Vector2& vec);
        void TranslateBy(const Vector3& vec);
        void TranslateBy(const Vector4& vec);
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