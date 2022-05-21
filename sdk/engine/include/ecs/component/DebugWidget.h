#pragma once

#include "Collider.h"
#include "DirectXMath.h"

namespace nc
{
    struct DebugWidget
    {
        DebugWidget(ColliderType colliderType, DirectX::FXMMATRIX matrix);
        std::string meshUid;
        DirectX::XMMATRIX transformationMatrix;
    };
}