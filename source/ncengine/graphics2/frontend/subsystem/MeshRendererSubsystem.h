#pragma once

#include "MeshRendererRenderState.h"
#include "ncengine/ecs/EcsFwd.h"

#include <vector>

namespace nc
{
class MeshRenderer2;
class Transform;

namespace graphics
{
class ToonRenderer;

/*
Produces a vector of transform matrices for MeshRenderers and their corresponding Entities.
*/
class MeshRendererSubsystem
{
    public:
        auto BuildState(ecs::ExplicitEcs<MeshRenderer2, ToonRenderer, Transform> ecs) -> MeshRendererRenderState;

    private:
        std::vector<MeshRendererData> m_rendererDataCache;
};
} // namespace graphics
} // namespace nc
