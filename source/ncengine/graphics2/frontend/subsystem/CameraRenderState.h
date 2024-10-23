#pragma once

#include "DirectXMath.h"

namespace nc::graphics
{
struct CameraRenderState
{
    DirectX::XMMATRIX viewProjection = DirectX::XMMatrixIdentity();
};
} // namespace nc::graphics
