#include "Vector3.h"
#include "Vector2.h"
#include "DirectXMath/DirectXMath.h"

namespace nc
{
    using namespace DirectX;

    /* Special Members */
    Vector3::Vector3()                          noexcept : m_x(0),       m_y(0),       m_z(0)       {}
    Vector3::Vector3(float x, float y, float z) noexcept : m_x(x),       m_y(y),       m_z(z)       {}
    Vector3::Vector3(const Vector3& vec)        noexcept : m_x(vec.m_x), m_y(vec.m_y), m_z(vec.m_z) {}
    Vector3::Vector3(const XMFLOAT3& vec)       noexcept : m_x(vec.x),   m_y(vec.y),   m_z(vec.z)   {}
    Vector3::Vector3(Vector3&& vec)             noexcept : m_x(vec.m_x), m_y(vec.m_y), m_z(vec.m_z) {}
    Vector3::Vector3(XMFLOAT3&& vec)            noexcept : m_x(vec.x),   m_y(vec.y),   m_z(vec.z)   {}
    Vector3::Vector3(float x, Vector2 yz)       noexcept : m_x(x),       m_y(yz.X()),  m_z(yz.Y())  {}
    Vector3::Vector3(Vector2 xy, float z)       noexcept : m_x(xy.X()),  m_y(xy.Y()),  m_z(z)       {}

    Vector3& Vector3::operator=(const Vector3& other) noexcept
        { m_x = other.m_x; m_y = other.m_y; m_z = other.m_z; return *this; }

    Vector3& Vector3::operator=(Vector3&& other) noexcept
        { m_x = other.m_x; m_y = other.m_y; m_z = other.m_z; return *this; }

    Vector3& Vector3::operator=(const DirectX::XMFLOAT3& xm) noexcept
        { m_x = xm.x; m_y = xm.y; m_z = xm.z; return *this; }

    Vector3& Vector3::operator=(DirectX::XMFLOAT3&& xm) noexcept 
        { m_x = xm.x; m_y = xm.y; m_z = xm.z; return *this; }

    /* Const Members */
    DirectX::XMFLOAT3 Vector3::GetXMFloat3() const noexcept
        { return DirectX::XMFLOAT3(m_x, m_y, m_z); }

    // Vector2 Vector3::ToVector2() const noexcept
    //     { return Vector2(m_x, m_y); }

    // Vector4 Vector3::ToVector4() const noexcept
    //     { return Vector4(m_x, m_y, m_z, 0); }

    /* Modifiers */

    // void Vector3::TranslateBy(const Vector2& vec) noexcept
    //     { m_x += vec.X(), m_y += vec.Y(); }

    

    

} // end namespace nc