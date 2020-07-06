#pragma once

#include "directx/math/DirectXMath.h"

namespace nc::graphics
{
    struct Material
    {
        DirectX::XMFLOAT3 color = {0.0f, 0.75f, 0.75f};
        float specularIntensity = 0.6;
        float specularPower = 32.0f;
        float padding[3];
    };
}