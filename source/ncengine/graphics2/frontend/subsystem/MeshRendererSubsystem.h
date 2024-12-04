#pragma once

#include "MeshRendererRenderState.h"
#include "TransformCache.h"
#include "InstanceCache.h"

#include "ncengine/ecs/EcsFwd.h"
#include "ncengine/utility/Signal.h"

namespace nc
{
class MeshRenderer2;
struct SystemEvents;
class Transform;

namespace graphics
{
class MeshRendererSubsystem
{
    public:
        explicit MeshRendererSubsystem(SystemEvents& events,
                                       uint32_t maxEntities,
                                       uint32_t maxMeshRenderers,
                                       uint32_t initialBatchSize);

        auto AddInstance(Entity entity,
                         MaterialInstanceHandle material,
                         const MaterialPasses passes,
                         const asset::MeshView& mesh) -> TransformDataHandle;

        void RemoveInstance(Entity entity,
                            uint32_t transformIndex,
                            uint64_t meshId,
                            MaterialPasses passes);

        void SetInstanceMesh(Entity entity,
                             uint32_t transformIndex,
                             MaterialInstanceHandle materialIndex,
                             MaterialPasses passes,
                             uint64_t oldMeshId,
                             const asset::MeshView& newMesh);

        void SetInstanceMaterial(Entity entity,
                                 uint32_t transformIndex,
                                 MaterialInstanceHandle materialIndex,
                                 MaterialPasses oldPasses,
                                 MaterialPasses newPasses,
                                 uint64_t meshId);

        auto BuildState(ecs::ExplicitEcs<MeshRenderer2, Transform> ecs) -> MeshRendererRenderState;
        void OnBeforeSceneLoad();

    private:
        TransformCache m_transformCache;
        InstanceCache<MeshRendererInstanceData> m_meshRendererInstanceCache;
        Connection m_rebuildStaticsConnection;

        void OnRebuildStatics();
};
} // namespace graphics
} // namespace nc
