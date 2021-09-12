#pragma once

#include "physics/Geometry.h"
#include "directx/math/DirectXMath.h"

namespace nc::camera
{
    void ClearMainCamera();
    void UpdateViewMatrix();
    void UpdateProjectionMatrix(float width, float height, float nearZ, float farZ);
    DirectX::FXMMATRIX GetViewMatrix();
    DirectX::FXMMATRIX GetProjectionMatrix();
    Frustum CalculateFrustum();
}