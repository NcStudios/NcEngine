#pragma once

#include "ncengine/ecs/Ecs.h"
#include "DirectXMath.h"

#include <span>

namespace nc::graphics
{
struct MeshRendererRenderState
{
    std::span<const DirectX::XMMATRIX> modelMatrices;
    std::span<const Entity> entities;
};
} // namespace nc::graphics
