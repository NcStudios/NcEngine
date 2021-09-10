#pragma once

#include "directx/math/DirectXMath.h"

namespace nc::camera
{
    void ClearMainCamera();
    DirectX::XMMATRIX CalculateViewMatrix();
    void SetProjectionMatrix(float width, float height, float nearZ, float farZ);
    DirectX::FXMMATRIX GetProjectionMatrix();
}