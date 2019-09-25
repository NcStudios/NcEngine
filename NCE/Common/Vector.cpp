#include "Vector.hpp"

namespace NCE::Common
{
    double Vector2::Magnitude() const { return sqrt((m_x * m_x) + (m_y * m_y)); }
    double Vector2::SquareMagnitude() const { return ((m_x * m_x) + (m_y * m_y)); }

    Vector2 Vector2::GetNormalized() const
    {
        double mag = Magnitude();
        return mag == 0 ? Zero() : Vector2(m_x / mag, m_y / mag);
    }

    Vector3 Vector2::ToVector3() const { return Vector3(m_x, m_y, 0); }
    Vector4 Vector2::ToVector4() const { return Vector4(m_x, m_y, 0, 0); }

    void Vector2::Normalize()
    {
        double mag = Magnitude();
        if (mag == 0) return;
        m_x /= mag;
        m_y /= mag;
    }

    void Vector2::TranslateBy(const Vector2& v_)
    {
        m_x += v_.GetX();
        m_y += v_.GetY();
    }

    Vector2 operator +(const Vector2 &lVal_, const Vector2 &rVal_)
    {
        return Vector2(lVal_.GetX() + rVal_.GetX(), lVal_.GetX() + rVal_.GetY());
    }

    Vector2 operator -(const Vector2 &lVal_, const Vector2 &rVal_)
    {
        return Vector2(lVal_.GetX() - rVal_.GetX(), lVal_.GetY() - rVal_.GetY());
    }

    Vector2 operator *(const Vector2 &v_, const double s_)
    {
        return Vector2(v_.GetX() * s_, v_.GetY() * s_);
    }

    Vector2 operator *(const double s_, const Vector2 &v_)
    {
        return Vector2(v_.GetX() * s_, v_.GetY() * s_);
    }

    Vector2 operator /(const Vector2 &v_, const double s_)
    {
        return Vector2(v_.GetX() / s_, v_.GetY() / s_);
    }

    Vector2 operator /(const double s_, const Vector2 &v_)
    {
        return Vector2(v_.GetX() / s_, v_.GetY() / s_);
    }

    bool operator ==(const Vector2 &lVal_, const Vector2 &rVal_)
    {
        //this needs to be done differently
        return Vector2::SquareMagnitude(lVal_ - rVal_) <= EPSILON;
    }

    bool operator !=(const Vector2 &lVal_, const Vector2 &rVal_)
    {
        return !operator==(lVal_, rVal_);
    }


    /* Vector4 */
    

    void Vector4::TranslateBy(const Vector4 &v_)
    {
        m_x += v_.GetX();
        m_y += v_.GetY();
        m_z += v_.GetZ();
        m_w += v_.GetW();
    }
}