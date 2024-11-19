#pragma once

#include "MaterialPassCache.h"
#include "MeshRendererCache.h"
#include "MeshRendererRenderState.h"
#include "TransformCache.h"
#include "InstanceCache.h"

#include "ncengine/ecs/EcsFwd.h"

#include <vector>

namespace nc
{
class MeshRenderer2;
class Transform;

namespace graphics
{
// todo should maybe just have 'handle' type to collect everything
struct AddInstanceResult
{
    uint32_t transformIndex;
    uint32_t instanceId;
};

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
                         const asset::MeshView& mesh) -> AddInstanceResult;

        // todo: shouldn't need entity + instance
        void RemoveInstance(uint32_t transformIndex,
                            uint32_t instance,
                            uint64_t meshId,
                            MaterialPasses passes);

        void SetInstanceMesh(Entity entity,
                             MaterialPasses passes,
                             const asset::MeshView& mesh);

        auto BuildState(ecs::ExplicitEcs<MeshRenderer2, Transform> ecs) -> MeshRendererRenderState;

    private:
        TransformCache m_transformCache;
        InstanceCache2 m_instanceCache;
};
} // namespace graphics
} // namespace nc
