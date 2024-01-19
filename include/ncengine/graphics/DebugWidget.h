#pragma once

#include "ncengine/physics/Collider.h"

#include "DirectXMath.h"

namespace nc::graphics
{
struct DebugWidget
{
    DebugWidget(physics::ColliderType colliderType, DirectX::FXMMATRIX matrix);
    DebugWidget(const std::string& mesh, DirectX::FXMMATRIX matrix)
        : meshUid{mesh}, transformationMatrix{matrix}
    {
    }

    std::string meshUid;
    DirectX::XMMATRIX transformationMatrix;
};
}
