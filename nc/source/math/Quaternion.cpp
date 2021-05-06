#include "math/Quaternion.h"
#include "directx/math/DirectXMath.h"
#include "debug/Utils.h"


namespace nc
{
    Quaternion::Quaternion()
        : x{0.0f}, y{0.0f}, z{0.0f}, w{1.0f}
    {
    }

    Quaternion::Quaternion(float X, float Y, float Z, float W)
        : x{X}, y{Y}, z{Z}, w{W}
    {
        IF_THROW
        (
            (x == 0.0f) && (y == 0.0f) && (z == 0.0f) && (w == 0.0f),
            "Quaternion - Cannot initialize all components with 0"
        );
    }

    Vector3 Quaternion::ToEulerAngles() const noexcept
    {
        float roll = std::atan2(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y));
        float sinp = 2.0f * (w * y - z * x);
        float pitch = std::abs(sinp) >= 1.0f ? std::copysign(std::numbers::pi / 2.0f, sinp) : std::asin(sinp);
        float yaw = std::atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));
        return Vector3{roll, pitch, yaw};
    }

    void Quaternion::ToAxisAngle(Vector3* axisOut, float* angleOut) const noexcept
    {
        DirectX::XMVECTOR axis_v;
        DirectX::XMQuaternionToAxisAngle(&axis_v, angleOut, DirectX::XMVectorSet(x, y, z, w));
        DirectX::XMStoreVector3(axisOut, axis_v);
    }

    Quaternion Quaternion::FromEulerAngles(const Vector3& angles)
    {
        auto quat_v = DirectX::XMQuaternionRotationRollPitchYaw(angles.x, angles.y, angles.z);
        auto out = Quaternion::Identity();
        DirectX::XMStoreQuaternion(&out, quat_v);
        return out;
    }

    Quaternion Quaternion::FromEulerAngles(float x, float y, float z)
    {
        auto quat_v = DirectX::XMQuaternionRotationRollPitchYaw(x, y, z);
        auto out = Quaternion::Identity();
        DirectX::XMStoreQuaternion(&out, quat_v);
        return out;
    }

    Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float angle)
    {
        auto axis_v = DirectX::XMVectorSet(axis.x, axis.y, axis.z, 0.0f);
        auto quat_v = DirectX::XMQuaternionRotationAxis(axis_v, angle);
        auto out = Quaternion::Identity();
        DirectX::XMStoreQuaternion(&out, quat_v);
        return out;
    }
}