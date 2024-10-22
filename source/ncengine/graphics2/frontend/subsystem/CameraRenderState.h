#pragma once

#include "DirectXMath.h"

namespace nc::graphics
{
struct CameraRenderState
{
    DirectX::XMMATRIX viewProjection;
};
} // namespace nc::graphics
