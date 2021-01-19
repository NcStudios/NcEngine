#include "Quaternion.h"
#include "directx/math/DirectXMath.h"
#include "DebugUtils.h"

namespace nc
{
    Quaternion::Quaternion(float X, float Y, float Z, float W)
            : x{X}, y{Y}, z{Z}, w{W}
    {
        IF_THROW
        (
            (x == 0.0f) && (y == 0.0f) && (z == 0.0f) && (w == 0.0f),
            "Quaternion - Cannot initialize all components with 0"
        );
    }

    Quaternion::Quaternion(const DirectX::XMFLOAT4& xmf4)
        : x{xmf4.x}, y{xmf4.y}, z{xmf4.z}, w{xmf4.w}
    {
        IF_THROW
        (
            (x == 0.0f) && (y == 0.0f) && (z == 0.0f) && (w == 0.0f),
            "Quaternion - Cannot initialize all components with 0"
        );
    }

    Quaternion::Quaternion(DirectX::XMFLOAT4&& xmf4)
        : x{xmf4.x}, y{xmf4.y}, z{xmf4.z}, w{xmf4.w}
    {
        IF_THROW
        (
            (x == 0.0f) && (y == 0.0f) && (z == 0.0f) && (w == 0.0f),
            "Quaternion - Cannot initialize all components with 0"
        );
    }

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