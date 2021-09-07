#pragma once

#include "component/Collider.h"
#include "directx/math/DirectXMath.h"

namespace nc
{
    struct DebugWidget
    {
        DebugWidget(ColliderType colliderType, DirectX::FXMMATRIX matrix);
        std::string meshUid;
        DirectX::XMMATRIX transformationMatrix;
    };
}