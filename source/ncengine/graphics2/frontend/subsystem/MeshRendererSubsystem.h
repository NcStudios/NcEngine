#pragma once

#include "MaterialPassCache.h"
#include "MeshRendererCache.h"
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
        explicit MeshRendererSubsystem(uint32_t maxMeshRenderers, std::span<const MaterialPass::type> passes);

        auto AddInstance(Entity entity,
                         MaterialInstanceHandle material,
                         const MaterialPasses passes,
                         const asset::MeshView& mesh) -> uint32_t;

        // todo: shouldn't need entity + instance
        void RemoveInstance(Entity entity,
                            uint32_t instance,
                            MaterialPasses passes);

        void SetInstanceMesh(Entity entity,
                             MaterialPasses passes,
                             const asset::MeshView& mesh);

        auto BuildState(ecs::ExplicitEcs<MeshRenderer2, Transform> ecs) -> MeshRendererRenderState;

    private:
        InstanceCache m_instanceCache;
        MaterialPassCache m_passCache;
};
} // namespace graphics
} // namespace nc
