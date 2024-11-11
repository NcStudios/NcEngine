#pragma once

#include "graphics2/ShaderTypes.h"
#include "ncengine/ecs/Ecs.h"
#include "DirectXMath.h"

#include <span>

namespace nc::graphics
{
struct MeshRendererRenderState
{
    std::span<const MeshRendererData> modelMatrices;
    std::span<const Entity> entities;
};
} // namespace nc::graphics
