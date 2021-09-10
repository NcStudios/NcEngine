#pragma once

#include "directx/math/DirectXMath.h"

namespace nc::camera
{
    void ClearMainCamera();
    void UpdateViewMatrix();
    void UpdateProjectionMatrix(float width, float height, float nearZ, float farZ);
    DirectX::FXMMATRIX GetViewMatrix();
    DirectX::FXMMATRIX GetProjectionMatrix();
}