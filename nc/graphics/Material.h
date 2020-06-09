#pragma once

#include "graphics/DirectXMath/Inc/DirectXMath.h"

namespace nc::graphics
{
    struct Material
    {
        DirectX::XMFLOAT3 color = {0.25f, 0.25f, 0.25f};
        float specularIntensity = 0.6;
        float specularPower = 30.0f;
        float padding[3];
    };
}