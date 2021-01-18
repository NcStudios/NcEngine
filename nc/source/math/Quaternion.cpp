#include "Quaternion.h"
#include "directx/math/DirectXMath.h"

namespace nc
{
    Quaternion::Quaternion(const DirectX::XMFLOAT4& xmf4) noexcept
        : x{xmf4.x}, y{xmf4.y}, z{xmf4.z}, w{xmf4.w}
    {}

    Quaternion::Quaternion(DirectX::XMFLOAT4&& xmf4) noexcept
        : x{xmf4.x}, y{xmf4.y}, z{xmf4.z}, w{xmf4.w}
    {}

    AxisAngle Quaternion::ToAxisAngle() const noexcept
    {
        DirectX::XMVECTOR axis_v;
        float angle;
        DirectX::XMQuaternionToAxisAngle(&axis_v, &angle, DirectX::XMVectorSet(x, y, z, w));
        DirectX::XMFLOAT3 axis;
        DirectX::XMStoreFloat3(&axis, axis_v);
        return AxisAngle{Vector3{axis}, angle};
    }

    Quaternion Quaternion::FromEulerAngles(const Vector3& angles)
    {
        auto quat_v = DirectX::XMQuaternionRotationRollPitchYaw(angles.x, angles.y, angles.z);
        DirectX::XMFLOAT4 xmf4;
        DirectX::XMStoreFloat4(&xmf4, quat_v);
        return Quaternion{xmf4};
    }

    Quaternion Quaternion::FromEulerAngles(float x, float y, float z)
    {
        auto quat_v = DirectX::XMQuaternionRotationRollPitchYaw(x, y, z);
        DirectX::XMFLOAT4 xmf4;
        DirectX::XMStoreFloat4(&xmf4, quat_v);
        return Quaternion{xmf4};
    }

    bool operator==(const Quaternion& lhs, const Quaternion& rhs)
    {
        //need to consider error
        return (math::FloatEqual(lhs.x, rhs.x) &&
                math::FloatEqual(lhs.y, rhs.y) &&
                math::FloatEqual(lhs.z, rhs.z) &&
                math::FloatEqual(lhs.w, rhs.w));
        //return ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z) && (lhs.w == rhs.w));
    }
}