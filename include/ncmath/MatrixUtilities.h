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
    constexpr auto selectX1Y1Z2 = XMVECTORU32{XM_SELECT_0, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0};
    const auto& x = XMVector3LengthSq(in.r[0]);
    const auto& y = XMVector3LengthSq(in.r[1]);
    const auto& z = XMVector3LengthSq(in.r[2]);
    const auto xyz = XMVectorSelect(XMVectorMergeXY(x, y), z, selectX1Y1Z2);
    return XMVectorSqrt(xyz);
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

inline auto ComposeMatrix(DirectX::FXMVECTOR scale,
                          DirectX::FXMVECTOR rotation,
                          DirectX::FXMVECTOR position) -> DirectX::XMMATRIX
{
    auto matrix = DirectX::XMMatrixScalingFromVector(scale) *
                  DirectX::XMMatrixRotationQuaternion(rotation);
    matrix.r[3] = position;
    return matrix;
}

inline auto ComposeMatrix(const Vector3& scale,
                          const Quaternion& rotation,
                          const Vector3& position) -> DirectX::XMMATRIX
{
    return ComposeMatrix(
        ToXMVector(scale),
        ToXMVector(rotation),
        ToXMVectorHomogeneous(position)
    );
}

inline float GetMaxScaleExtent(DirectX::FXMMATRIX matrix)
{
    auto xExtent_v = DirectX::XMVector3Dot(matrix.r[0], matrix.r[0]);
    auto yExtent_v = DirectX::XMVector3Dot(matrix.r[1], matrix.r[1]);
    auto zExtent_v = DirectX::XMVector3Dot(matrix.r[2], matrix.r[2]);
    auto maxExtent_v = DirectX::XMVectorMax(xExtent_v, DirectX::XMVectorMax(yExtent_v, zExtent_v));
    return sqrt(DirectX::XMVectorGetX(maxExtent_v));
}
} // namespace nc
