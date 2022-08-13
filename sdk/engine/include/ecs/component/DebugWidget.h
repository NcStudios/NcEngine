#pragma once

#include "physics/Collider.h"

#include "DirectXMath.h"

namespace nc
{
struct DebugWidget
{
    DebugWidget(physics::ColliderType colliderType, DirectX::FXMMATRIX matrix);
    std::string meshUid;
    DirectX::XMMATRIX transformationMatrix;
};
}
