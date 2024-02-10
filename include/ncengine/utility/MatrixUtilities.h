/**
 * @file MatrixUtilities.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncmath/Vector.h"
#include "ncmath/Quaternion.h"
#include "DirectXMath.h"

namespace nc
{
struct DecomposedMatrixXM
{
    DirectX::XMVECTOR scale;
    DirectX::XMVECTOR rotation;
    DirectX::XMVECTOR position;
};

inline auto ToVector3(DirectX::FXMVECTOR in) noexcept -> Vector3
{
    auto out = Vector3{};
    DirectX::XMStoreVector3(&out, in);
    return out;
}

inline auto ToQuaternion(DirectX::FXMVECTOR in) noexcept -> Quaternion
{
    auto out = Quaternion::Identity();
    DirectX::XMStoreQuaternion(&out, in);
    return out;
}

inline auto DecomposeMatrix(DirectX::FXMMATRIX in) noexcept -> DecomposedMatrixXM
{
    auto out = DecomposedMatrixXM{};
    DirectX::XMMatrixDecompose(&out.scale, &out.rotation, &out.position, in);
    return out;
}

inline auto DecomposeRotation(DirectX::FXMMATRIX in) noexcept -> DirectX::XMVECTOR
{
    auto [scl, rot, pos] = DecomposeMatrix(in);
    return rot;
}

inline auto DecomposeScale(DirectX::FXMMATRIX in) noexcept -> DirectX::XMVECTOR
{
    using namespace DirectX;
    auto out = XMVectorSplatX(XMVector3Length(in.r[0]));
    out = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_0W>(out, XMVector3Length(in.r[1]));
    out = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_0W>(out, XMVector3Length(in.r[2]));
    return out;
}

inline DirectX::XMVECTOR ToXMVector(const Vector3& v)
{
    return DirectX::XMLoadVector3(&v);
}

inline DirectX::XMVECTOR ToXMVectorHomogeneous(const Vector3& v)
{
    return DirectX::XMVectorSetW(ToXMVector(v), 1.0f);
}

inline DirectX::XMVECTOR ToXMVector(const Quaternion& q)
{
    return DirectX::XMLoadQuaternion(&q);
}

inline DirectX::XMMATRIX ToTransMatrix(const Vector3& v)
{
    return DirectX::XMMatrixTranslation(v.x, v.y, v.z);
}

inline DirectX::XMMATRIX ToScaleMatrix(const Vector3& v)
{
    return DirectX::XMMatrixScaling(v.x, v.y, v.z);
}

inline DirectX::XMMATRIX ToRotMatrix(const Vector3& v)
{
    return DirectX::XMMatrixRotationRollPitchYaw(v.x, v.y, v.z);
}

inline DirectX::XMMATRIX ToRotMatrix(const Quaternion& q)
{
    return DirectX::XMMatrixRotationQuaternion(ToXMVector(q));
}

inline DirectX::XMMATRIX ToRotMatrix(const Vector3& a, float r)
{
    return DirectX::XMMatrixRotationAxis(ToXMVector(a), r);
}

inline auto ComposeMatrix(const Vector3& scale, const Quaternion& rot, const Vector3& pos) -> DirectX::XMMATRIX
{
    return ToScaleMatrix(scale) * ToRotMatrix(rot) * ToTransMatrix(pos);
}
} // namespace nc
