#pragma once

#include "directx/math/DirectXMath.h"

namespace nc { class Transform; }

namespace nc::graphics::vulkan
{
    struct TransformMatrices
    {
        DirectX::XMMATRIX modelView;
        DirectX::XMMATRIX model;
    };

    TransformMatrices GetMatrices(Transform* transform);
}