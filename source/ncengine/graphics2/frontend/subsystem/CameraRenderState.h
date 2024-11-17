#pragma once

#include "DirectXMath.h"

namespace nc::graphics
{
struct CameraRenderState
{
    DirectX::XMMATRIX viewProjection = DirectX::XMMatrixIdentity();
    Vector3 position = Vector3::One();
};
} // namespace nc::graphics
