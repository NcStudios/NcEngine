#include "Vector3.h"
#include "Vector2.h"
#include "directx/math/DirectXMath.h"

namespace nc
{
    using namespace DirectX;

    /* Special Members */
    Vector3::Vector3() noexcept
        : x(0), y(0), z(0)
    {
    }

    Vector3::Vector3(float x, float y, float z) noexcept
        : x(x), y(y), z(z)
    {
    }

    Vector3::Vector3(const Vector3& vec) noexcept
        : x(vec.x), y(vec.y), z(vec.z)
    {
    }

    Vector3::Vector3(const XMFLOAT3& vec) noexcept
        : x(vec.x), y(vec.y), z(vec.z)
    {
    }

    Vector3::Vector3(Vector3&& vec) noexcept
        : x(vec.x), y(vec.y), z(vec.z) 
    {
    }

    Vector3::Vector3(XMFLOAT3&& vec) noexcept
        : x(vec.x), y(vec.y), z(vec.z)
    {
    }

    Vector3::Vector3(float x, Vector2 yz) noexcept
        : x(x), y(yz.x), z(yz.y)
    {
    }

    Vector3::Vector3(Vector2 xy, float z) noexcept
        : x(xy.x), y(xy.y), z(z)
    {
    }

    Vector3& Vector3::operator=(const Vector3& other) noexcept
        { x = other.x; y = other.y; z = other.z; return *this; }

    Vector3& Vector3::operator=(Vector3&& other) noexcept
        { x = other.x; y = other.y; z = other.z; return *this; }

    Vector3& Vector3::operator=(const DirectX::XMFLOAT3& xm) noexcept
        { x = xm.x; y = xm.y; z = xm.z; return *this; }

    Vector3& Vector3::operator=(DirectX::XMFLOAT3&& xm) noexcept 
        { x = xm.x; y = xm.y; z = xm.z; return *this; }

    /* Const Members */
    DirectX::XMFLOAT3 Vector3::ToXMFloat3() const noexcept
        { return DirectX::XMFLOAT3(x, y, z); }
} // end namespace nc