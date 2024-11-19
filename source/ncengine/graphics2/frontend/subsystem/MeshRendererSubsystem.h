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
        explicit MeshRendererSubsystem(SystemEvents& events, uint32_t maxMeshRenderers);

        auto AddInstance(Entity entity,
                         MaterialInstanceHandle material,
                         const MaterialPasses passes,
                         const asset::MeshView& mesh) -> uint32_t; // return transformIndex I guess ??

        void RemoveInstance(Entity entity,
                            uint32_t transformIndex,
                            uint64_t meshId,
                            MaterialPasses passes);

        void SetInstanceMesh(Entity entity,
                             uint32_t transformIndex,
                             MaterialInstanceHandle materialIndex,
                             MaterialPasses oldPasses,
                             MaterialPasses newPasses,
                             uint64_t oldMeshId,
                             const asset::MeshView& newMesh);

        auto BuildState(ecs::ExplicitEcs<MeshRenderer2, Transform> ecs) -> MeshRendererRenderState;

    private:
        TransformCache m_transformCache;
        InstanceCache2 m_instanceCache;
        Connection m_rebuildStaticsConnection;

        void OnRebuildStatics();
};
} // namespace graphics
} // namespace nc
