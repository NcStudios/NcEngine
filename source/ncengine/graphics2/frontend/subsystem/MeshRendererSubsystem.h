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
        /** @todo 794 Use more passes as they become available. For now we just have one dummy pass. */
        explicit MeshRendererSubsystem()
            : m_passCache{std::vector{MaterialPass::Toon}}
        {
        }

        auto BuildState(ecs::ExplicitEcs<MeshRenderer2, Transform> ecs) -> MeshRendererRenderState;

    private:
        std::vector<MeshRendererData> m_rendererDataCache;
        MaterialPassCache m_passCache;
};
} // namespace graphics
} // namespace nc
