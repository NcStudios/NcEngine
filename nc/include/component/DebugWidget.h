#pragma once

#include "component/Collider.h"
#include "directx/math/DirectXMath.h"
#include <string>

namespace nc
{
    struct DebugWidget
    {
        DebugWidget(::nc::ColliderType colliderType, DirectX::FXMMATRIX matrix);
        std::string meshUid;
        DirectX::XMMATRIX transformationMatrix;
    };
}