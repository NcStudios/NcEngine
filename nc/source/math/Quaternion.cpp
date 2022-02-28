#include "math/Quaternion.h"
#include "directx/Inc/DirectXMath.h"
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
        NC_ASSERT
        (
            (x != 0.0f) || (y != 0.0f) || (z != 0.0f) || (w != 0.0f),
            "Quaternion - Cannot initialize all components with 0"
        );
    }

    Vector3 Quaternion::ToEulerAngles() const noexcept
    {
        float roll = std::atan2(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y));
        float sinp = 2.0f * (w * y - z * x);
        float pitch = std::abs(sinp) >= 1.0f ? std::copysign(std::numbers::pi_v<float> / 2.0f, sinp) : std::asin(sinp);
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

    Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float radians)
    {
        NC_ASSERT(axis != Vector3::Zero(), "Axis cannot be zero");
        auto axis_v = DirectX::XMVectorSet(axis.x, axis.y, axis.z, 0.0f);
        auto quat_v = DirectX::XMQuaternionRotationAxis(axis_v, radians);
        auto out = Quaternion::Identity();
        DirectX::XMStoreQuaternion(&out, quat_v);
        return out;
    }

    Quaternion Normalize(const Quaternion& quat)
    {
        float magInv = 1.0f / sqrt(quat.x * quat.x + quat.y * quat.y + quat.z * quat.z + quat.w * quat.w);
        return Quaternion{magInv * quat.x, magInv * quat.y, magInv * quat.z, magInv * quat.w};
    }

    Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs)
    {
        auto lhs_v = DirectX::XMVectorSet(lhs.x, lhs.y, lhs.z, lhs.w);
        auto rhs_v = DirectX::XMVectorSet(rhs.x, rhs.y, rhs.z, rhs.w);
        auto out_v = DirectX::XMQuaternionMultiply(lhs_v, rhs_v); // returns rhs_v * lhs_v
        auto out = Quaternion::Identity();
        DirectX::XMStoreQuaternion(&out, out_v);
        return out;
    }

    Quaternion Difference(const Quaternion& lhs, const Quaternion& rhs)
    {
        auto lhs_v = DirectX::XMVectorSet(lhs.x, lhs.y, lhs.z, lhs.w);
        auto rhs_v = DirectX::XMVectorSet(rhs.x, rhs.y, rhs.z, rhs.w);
        lhs_v = DirectX::XMQuaternionConjugate(lhs_v);
        auto out_v = DirectX::XMQuaternionMultiply(lhs_v, rhs_v);
        auto out = Quaternion::Identity();
        DirectX::XMStoreQuaternion(&out, out_v);
        return out;
    }

    Quaternion Slerp(const Quaternion& lhs, const Quaternion& rhs, float factor)
    {
        auto lhs_v = DirectX::XMVectorSet(lhs.x, lhs.y, lhs.z, lhs.w);
        auto rhs_v = DirectX::XMVectorSet(rhs.x, rhs.y, rhs.z, rhs.w);
        auto out_v = DirectX::XMQuaternionSlerp(lhs_v, rhs_v, factor);
        auto out = Quaternion::Identity();
        DirectX::XMStoreQuaternion(&out, out_v);
        return out;
    }

    Quaternion Scale(const Quaternion& quat, float factor)
    {
        auto quat_v = DirectX::XMVectorSet(quat.x, quat.y, quat.z, quat.w);
        auto out_v = DirectX::XMQuaternionSlerp(DirectX::g_XMIdentityR3, quat_v, factor);
        auto out = Quaternion::Identity();
        DirectX::XMStoreQuaternion(&out, out_v);
        return out;
    }
}