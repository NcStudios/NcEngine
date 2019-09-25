#include "Vector.hpp"

namespace NCE::Common
{
    void Vector2::Set(double x_, double y_)
    {
        m_x = x_;
        m_y = y_;
    }

    double Vector2::Magnitude() { return sqrt((m_x * m_x) + (m_y * m_y)); }
    double Vector2::SquareMagnitude() { return ((m_x * m_x) + (m_y * m_y)); }
    double Vector2::SquareMagnitude(const Vector2 &v_) { return ((v_.GetX() * v_.GetX()) + (v_.GetY() * v_.GetY())); }

    void Vector2::Normalize()
    {
        double mag = Magnitude();
        if (mag == 0) return;
        m_x /= mag;
        m_y /= mag;
    }

    Vector2 Vector2::GetNormalized()
    {
        double mag = Magnitude();
        return mag == 0 ?  Zero() : Vector2(m_x / mag, m_y / mag);
    }

    Vector2 operator +(const Vector2 & lVal_, const Vector2& rVal_)
    {
        return Vector2(lVal_.GetX() + rVal_.GetX(), lVal_.GetX() + rVal_.GetY());
    }

    Vector2 operator -(const Vector2 & lVal_, const Vector2& rVal_)
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
    
    void Vector4::Set(double x_, double y_, double z_, double w_)
    {
        m_x = x_;
        m_y = y_;
        m_z = z_;
        m_w = w_;
    }
}