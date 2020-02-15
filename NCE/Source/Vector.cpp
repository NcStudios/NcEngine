#include "../Include/Vector.h"

namespace nc{

    /* VECTOR 2 */
double Vector2::Magnitude() const { return sqrt( (m_x * m_x) + (m_y * m_y) ); }

double Vector2::SquareMagnitude() const { return SquareMagnitude(*this); }

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
    if (mag == 0)
        return;
    m_x /= mag;
    m_y /= mag;
}

void Vector2::TranslateBy(const Vector2& vec) { m_x += vec.m_x, m_y += vec.m_y; }
    /* END VECTOR 2 */


    /* VECTOR 3 */
double Vector3::Magnitude() const { return sqrt( (m_x * m_x) + (m_y * m_y) + (m_z * m_z) ); }

double Vector3::SquareMagnitude() const { return SquareMagnitude(*this); }

Vector3 Vector3::GetNormalized() const 
{
    double mag = Magnitude();
    return mag == 0 ? Zero() : Vector3(m_x / mag, m_y / mag, m_z / mag);
}

Vector2 Vector3::ToVector2() const { return Vector2(m_x, m_y); }

Vector4 Vector3::ToVector4() const { return Vector4(m_x, m_y, m_z, 0); }

void Vector3::Normalize()
{
    double mag = Magnitude();
    if (mag == 0)
        return;
    m_x /= mag, m_y /= mag, m_z /= mag;
}

void Vector3::TranslateBy(const Vector2& vec) { m_x += vec.GetX(), m_y += vec.GetY(); }

void Vector3::TranslateBy(const Vector3& vec) { m_x += vec.m_x, m_y += vec.m_y, m_z += vec.m_z; }
    /* END VECTOR 3 */


    /* VECTOR 4 */
double Vector4::Magnitude() const { return sqrt( (m_x * m_x) + (m_y * m_y) + (m_z * m_z) + (m_w * m_w) ); }

double Vector4::SquareMagnitude() const { return SquareMagnitude(*this); }

Vector4 Vector4::GetNormalized() const 
{
    double mag = Magnitude();
    return mag == 0 ? Zero() : Vector4(m_x / mag, m_y / mag, m_z / mag, m_w / mag);
}

Vector2 Vector4::ToVector2() const { return Vector2(m_x, m_y); }
Vector3 Vector4::ToVector3() const { return Vector3(m_x, m_y, m_z); }

void Vector4::Normalize()
{
    double mag = Magnitude();
    if (mag == 0)
        return;
    m_x /= mag, m_y /= mag, m_z /= mag, m_w /= mag;
}

void Vector4::TranslateBy(const Vector2& vec) { m_x += vec.GetX(), m_y += vec.GetY(); }
void Vector4::TranslateBy(const Vector3& vec) { m_x += vec.GetX(), m_y += vec.GetY(), m_z += vec.GetZ(); }
void Vector4::TranslateBy(const Vector4& vec) { m_x += vec.m_x, m_y += vec.m_y, m_z += vec.m_z, m_w += vec.m_w; }
    /* END VECTOR 4 */


    /* OPERATORS */

    /* VECTOR 2 OPERATORS */
Vector2 operator +(const Vector2& lhs, const Vector2& rhs)
{
    return Vector2(lhs.GetX() + rhs.GetX(), lhs.GetY() + rhs.GetY());
}

Vector2 operator -(const Vector2& lhs, const Vector2& rhs)
{
    return Vector2(lhs.GetX() - rhs.GetX(), lhs.GetY() - rhs.GetY());
}

Vector2 operator *(const Vector2& vec, const double scalar)
{
    return Vector2(vec.GetX() * scalar, vec.GetY() * scalar);
}

Vector2 operator /(const Vector2& vec, const double scalar)
{
    return Vector2(vec.GetX() / scalar, vec.GetY() / scalar);
}

bool operator ==(const Vector2& lhs, const Vector2& rhs)
{
    return Vector2::SquareMagnitude(lhs - rhs) <= EPSILON;
}

bool operator !=(const Vector2& lhs, const Vector2& rhs)
{
    return !(lhs == rhs);
}

    /* VECTOR 3 OPERATORS */

Vector3 operator +(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3(lhs.GetX() + rhs.GetX(), lhs.GetY() + rhs.GetY(), lhs.GetZ() + rhs.GetZ());
}

Vector3 operator -(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3(lhs.GetX() - rhs.GetX(), lhs.GetY() - rhs.GetY(), lhs.GetZ() - rhs.GetZ());
}

Vector3 operator *(const Vector3& vec, const double scalar)
{
    return Vector3(vec.GetX() * scalar, vec.GetY() * scalar, vec.GetZ() * scalar);
}

Vector3 operator /(const Vector3& vec, const double scalar)
{
    return Vector3(vec.GetX() / scalar, vec.GetY() / scalar, vec.GetZ() / scalar);
}

bool operator ==(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3::SquareMagnitude(lhs - rhs) <= EPSILON;
}

bool operator !=(const Vector3& lhs, const Vector3& rhs)
{
    return !(lhs == rhs);
}

    /* VECTOR 4 OPERATORS */

Vector4 operator +(const Vector4& lhs, const Vector4& rhs)
{
    return Vector4(lhs.GetX() + rhs.GetX(), lhs.GetY() + rhs.GetY(), lhs.GetZ() + rhs.GetZ(), lhs.GetW() + rhs.GetW());
}

Vector4 operator -(const Vector4& lhs, const Vector4& rhs)
{
    return Vector4(lhs.GetX() - rhs.GetX(), lhs.GetY() - rhs.GetY(), lhs.GetZ() - rhs.GetZ(), lhs.GetW() - rhs.GetW());
}

Vector4 operator *(const Vector4& vec, const double scalar)
{
    return Vector4(vec.GetX() * scalar, vec.GetY() * scalar, vec.GetZ() * scalar, vec.GetW() * scalar);
}

Vector4 operator /(const Vector4& vec, const double scalar)
{
    return Vector4(vec.GetX() / scalar, vec.GetY() / scalar, vec.GetZ() / scalar, vec.GetW() / scalar);
}

bool operator ==(const Vector4& lhs, const Vector4& rhs)
{
    return Vector4::SquareMagnitude(lhs - rhs) <= EPSILON;
}

bool operator !=(const Vector4& lhs, const Vector4& rhs)
{
    return !(lhs == rhs);
}

} //end namespace nc