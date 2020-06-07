#pragma once

namespace nc::graphics
{
    struct Material
    {
        DirectX::XMFLOAT3 color = {255, 0, 0};
        float specularIntensity = 0.6;
        float specularPower = 30.0f;
        float padding[3];
    };
}