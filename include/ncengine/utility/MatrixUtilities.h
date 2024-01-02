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

inline auto DecomposeRotation(DirectX::FXMMATRIX in) noexcept -> DirectX::XMVECTOR
{
    DirectX::XMVECTOR scl, rot, pos;
    DirectX::XMMatrixDecompose(&scl, &rot, &pos, in);
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
} // namespace nc
