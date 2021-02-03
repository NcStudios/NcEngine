#pragma once

#include "directx/math/DirectXMath.h"

namespace nc::graphics
{
    struct MaterialProperties
    {
        DirectX::XMFLOAT3 color = {0.25f, 0.25f, 0.25f};
        float specularIntensity = 0.6;
        float specularPower = 32.0f;
        float xTiling = 1.0f;
        float yTiling = 1.0f;
        float padding[1];
    };
}