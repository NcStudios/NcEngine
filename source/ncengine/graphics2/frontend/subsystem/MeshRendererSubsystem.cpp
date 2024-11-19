#include "MeshRendererSubsystem.h"
#include "ncengine/Events.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer2.h"
#include "ncengine/graphics/GraphicsUtility.h"

#include "ncengine/debug/Profile.h"

namespace nc::graphics
{
MeshRendererSubsystem::MeshRendererSubsystem(SystemEvents& events, uint32_t maxMeshRenderers)
    : m_transformCache{maxMeshRenderers},
      m_instanceCache{},
      m_rebuildStaticsConnection{events.rebuildStatics.Connect(this, &MeshRendererSubsystem::OnRebuildStatics)}
{
    MeshRenderer2::s_subsystem = this;
}

auto MeshRendererSubsystem::AddInstance(Entity entity,
                                        MaterialInstanceHandle material,
                                        MaterialPasses passes,
                                        const asset::MeshView& mesh) -> uint32_t
{
    const auto transformIndex = m_transformCache.AddInstance(entity);
    m_instanceCache.StageAdd(entity.Index(), transformIndex, material, passes, mesh);
    return transformIndex;
}

void MeshRendererSubsystem::RemoveInstance(Entity entity,
                                           uint32_t transformIndex,
                                           uint64_t meshId,
                                           MaterialPasses passes)
{
    m_transformCache.RemoveInstance(transformIndex);
    m_instanceCache.StageRemove(entity.Index(), meshId, passes);
}

void MeshRendererSubsystem::SetInstanceMesh(Entity entity,
                                            uint32_t transformIndex,
                                            MaterialInstanceHandle materialIndex,
                                            MaterialPasses oldPasses,
                                            MaterialPasses newPasses,
                                            uint64_t oldMeshId,
                                            const asset::MeshView& newMesh)
{
    m_instanceCache.UpdateInstance(
        entity.Index(),
        transformIndex,
        materialIndex,
        oldPasses,
        newPasses,
        oldMeshId,
        newMesh
    );
}

auto MeshRendererSubsystem::BuildState(ecs::ExplicitEcs<MeshRenderer2, Transform> ecs) -> MeshRendererRenderState
{
    NC_PROFILE_SCOPE("MeshRendererSubsystem::BuildState()", ProfileCategory::Rendering);
    m_transformCache.UpdateMatrices(ecs);
    m_instanceCache.CommitPendingChanges();
    return MeshRendererRenderState{
        .transformData = m_transformCache.BuildState(),
        .instanceData = m_instanceCache.BuildState(),
        .passBatches = m_instanceCache.BuildBatches(GetImplementedMaterialPassFlags())
    };
}

void MeshRendererSubsystem::OnRebuildStatics()
{
    m_transformCache.MarkStaticsDirty();
}
} // namespace nc::graphics
