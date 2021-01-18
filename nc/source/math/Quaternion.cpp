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

    void Quaternion::ToAxisAngle(Vector3* axisOut, float* angleOut) const noexcept
    {
        DirectX::XMVECTOR axis_v;
        DirectX::XMQuaternionToAxisAngle(&axis_v, angleOut, DirectX::XMVectorSet(x, y, z, w));
        DirectX::XMFLOAT3 axis;
        DirectX::XMStoreFloat3(&axis, axis_v);
        *axisOut = axis;
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

    Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float angle)
    {
        auto axis_v = DirectX::XMVectorSet(axis.x, axis.y, axis.z, 0.0f);
        auto quat_v = DirectX::XMQuaternionRotationAxis(axis_v, angle);
        DirectX::XMFLOAT4 quat;
        DirectX::XMStoreFloat4(&quat, quat_v);
        return Quaternion{quat};
    }
}