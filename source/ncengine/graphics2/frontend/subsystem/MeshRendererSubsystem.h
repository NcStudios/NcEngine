#pragma once

#include "MaterialPassCache.h"
#include "MeshRendererRenderState.h"
#include "ncengine/ecs/EcsFwd.h"

#include <vector>

namespace nc
{
class MeshRenderer2;
class Transform;

namespace graphics
{
/*
Produces a vector of transform matrices for MeshRenderers and their corresponding Entities.
*/
class MeshRendererSubsystem
{
    public:
        explicit MeshRendererSubsystem(std::span<const MaterialPass::type> passes)
            : m_passCache{passes}
        {
        }

        auto BuildState(ecs::ExplicitEcs<MeshRenderer2, Transform> ecs) -> MeshRendererRenderState;

    private:
        std::vector<MeshRendererData> m_rendererDataCache;
        MaterialPassCache m_passCache;
};
} // namespace graphics
} // namespace nc
