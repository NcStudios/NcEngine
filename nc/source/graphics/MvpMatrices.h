#pragma once

#include "directx/math/DirectXMath.h"

namespace nc::graphics
{
    struct MvpMatrices
    {
        DirectX::XMMATRIX modelView;
        DirectX::XMMATRIX modelViewProjection;
    };
}