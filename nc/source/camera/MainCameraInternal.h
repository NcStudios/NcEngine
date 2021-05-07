#pragma once

#include "directx/math/DirectXMath.h"

namespace nc::camera
{
    void ClearMainCamera();
    DirectX::XMMATRIX CalculateViewMatrix();
}