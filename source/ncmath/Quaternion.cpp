#include "ncmath/Quaternion.h"
#include "ncmath/MatrixUtilities.h"
#include "ncutility/NcError.h"

#include "DirectXMath.h"

using namespace DirectX;

namespace
{
auto GetRotation4x4(FXMVECTOR quat) -> XMFLOAT4X4
{
    const auto matrixXM = XMMatrixRotationQuaternion(quat);
    auto matrix = XMFLOAT4X4{};
    XMStoreFloat4x4(&matrix, matrixXM);
    return matrix;
}

auto IsPitchNear90(float pitch) -> bool
{
    constexpr auto epsilon = 5.0e-5f;
    constexpr auto pi_2 = std::numbers::pi_v<float> / 2.0f;
    const auto absPitch = std::fabs(pitch);
    return std::fabs(absPitch - pi_2) <= epsilon;
}
} // anonymous namespace

namespace nc
{
Quaternion::Quaternion(float X, float Y, float Z, float W)
    : x{X}, y{Y}, z{Z}, w{W}
{
    NC_ASSERT
    (
        (x != 0.0f) || (y != 0.0f) || (z != 0.0f) || (w != 0.0f),
        "Quaternion - Cannot initialize all components with 0"
    );
}

auto Quaternion::ToEulerAngles() const noexcept -> Vector3
{
    const auto m = GetRotation4x4(ToXMVector(*this));
    auto out = Vector3{};
    auto& [pitch, yaw, roll] = out;

    pitch = -std::asin(Clamp(m._32, -1.0f, 1.0f));
    if (!IsPitchNear90(pitch))
    {
        yaw = std::atan2(m._31, m._33);
        roll = std::atan2(m._12, m._22);
    }
    else
    {
        // Gimbal lock case
        yaw = std::atan2(-m._13, m._11);
        roll = 0.0f;
    }

    return out;
}

void Quaternion::ToAxisAngle(Vector3* axisOut, float* angleOut) const noexcept
{
    auto axis_v = XMVECTOR{};
    XMQuaternionToAxisAngle(&axis_v, angleOut, ToXMVector(*this));
    axis_v = XMVector3Normalize(axis_v);
    XMStoreVector3(axisOut, axis_v);
}

auto Quaternion::FromEulerAngles(const Vector3& angles) -> Quaternion
{
    return FromEulerAngles(angles.x, angles.y, angles.z);
}

auto Quaternion::FromEulerAngles(float x, float y, float z) -> Quaternion
{
    const auto quat_v = XMQuaternionRotationRollPitchYaw(x, y, z);
    return ToQuaternion(quat_v);
}

auto Quaternion::FromAxisAngle(const Vector3& axis, float radians) -> Quaternion
{
    NC_ASSERT(axis != Vector3::Zero(), "Invalid Rotation Axis");

    const auto quat_v = XMQuaternionRotationAxis(ToXMVector(axis), radians);
    return ToQuaternion(quat_v);
}

auto Normalize(const Quaternion& quat) -> Quaternion
{
    const auto magInv = 1.0f / sqrt(quat.x * quat.x + quat.y * quat.y + quat.z * quat.z + quat.w * quat.w);
    return Quaternion{magInv * quat.x, magInv * quat.y, magInv * quat.z, magInv * quat.w};
}

auto Multiply(const Quaternion& lhs, const Quaternion& rhs) -> Quaternion
{
    const auto lhs_v = ToXMVector(lhs);
    const auto rhs_v = ToXMVector(rhs);
    const auto out_v = XMQuaternionMultiply(lhs_v, rhs_v); // returns rhs_v * lhs_s
    return ToQuaternion(out_v);
}

auto Difference(const Quaternion& lhs, const Quaternion& rhs) -> Quaternion
{
    const auto lhs_v = XMQuaternionConjugate(ToXMVector(lhs));
    const auto rhs_v = ToXMVector(rhs);
    const auto out_v = XMQuaternionMultiply(lhs_v, rhs_v);
    return ToQuaternion(out_v);
}

auto Slerp(const Quaternion& lhs, const Quaternion& rhs, float factor) -> Quaternion
{
    const auto lhs_v = ToXMVector(lhs);
    const auto rhs_v = ToXMVector(rhs);
    const auto out_v = XMQuaternionSlerp(lhs_v, rhs_v, factor);
    return ToQuaternion(out_v);
}

auto Scale(const Quaternion& quat, float factor) -> Quaternion
{
    const auto quat_v = ToXMVector(quat);
    const auto out_v = XMQuaternionSlerp(g_XMIdentityR3, quat_v, factor);
    return ToQuaternion(out_v);
}
} // namespace nc
