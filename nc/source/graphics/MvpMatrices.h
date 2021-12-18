#pragma once

#include "directx/Inc/DirectXMath.h"

namespace nc::graphics
{
    struct MvpMatrices
    {
        DirectX::XMMATRIX modelView;
        DirectX::XMMATRIX modelViewProjection;
    };
}