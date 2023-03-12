#pragma once

#include "DirectXMath.h"
#include "ncmath/Geometry.h"
#include "ncmath/Vector.h"

namespace nc::graphics
{
struct CameraFrontendState
{
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
    Vector3 position;
    Frustum frustum;
};
} // namespace nc::graphics
