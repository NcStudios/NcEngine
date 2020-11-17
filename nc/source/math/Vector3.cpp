#include "Vector3.h"
#include "directx/math/DirectXMath.h"

namespace nc
{
    Vector3::Vector3(const DirectX::XMFLOAT3& xm) noexcept
        : x{xm.x}, y{xm.y}, z{xm.z} {}

    Vector3::Vector3(DirectX::XMFLOAT3&& xm) noexcept
        : x{xm.x}, y{xm.y}, z{xm.z} {}

    Vector3& Vector3::operator=(const DirectX::XMFLOAT3& xm) noexcept
    {
        x = xm.x;
        y = xm.y;
        z = xm.z;
        return *this;
    }

    Vector3& Vector3::operator=(DirectX::XMFLOAT3&& xm) noexcept 
    {
        x = xm.x;
        y = xm.y;
        z = xm.z;
        return *this;
    }

    DirectX::XMFLOAT3 Vector3::ToXMFloat3() const noexcept
    {
        return DirectX::XMFLOAT3{x, y, z};
    }
} // end namespace nc