#pragma once
#include "directx/math/DirectXMath.h"

namespace nc::graphics
{
    /** @todo We probably don't want the viewProjection in this struct. We only need
     *  1 copy, not 1 per renderer. */

    struct ObjectData
    {
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX modelView;
        DirectX::XMMATRIX viewProjection;

        uint32_t baseColorIndex; // Todo: Make this more generic for materials
        uint32_t normalIndex;  // Todo: Make this more generic for materials
        uint32_t roughnessIndex;  // Todo: Make this more generic for materials

        uint32_t isInitialized = 0;
    };
}