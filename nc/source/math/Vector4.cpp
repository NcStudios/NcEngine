#include "Vector4.h"
#include "directx/math/DirectXMath.h"

namespace nc
{
    Vector4::Vector4(const DirectX::XMFLOAT4& xm) noexcept
        : x{xm.x}, y{xm.y}, z{xm.z}, w{xm.w}
    {}

    Vector4::Vector4(DirectX::XMFLOAT4&& xm) noexcept
        : x{xm.x}, y{xm.y}, z{xm.z}, w{xm.w}
    {}

    DirectX::XMFLOAT4 Vector4::ToXMFloat4() const noexcept
    {
        return DirectX::XMFLOAT4{x, y, z, w};
    }
}