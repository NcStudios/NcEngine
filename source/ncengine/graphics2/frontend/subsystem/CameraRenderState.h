#pragma once

#include "DirectXMath.h"

namespace nc::graphics
{
struct CameraRenderState
{
    DirectX::XMMATRIX viewProjection = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX invProjection = DirectX::XMMatrixIdentity();
    Vector3 position = Vector3::Zero();
};
} // namespace nc::graphics
