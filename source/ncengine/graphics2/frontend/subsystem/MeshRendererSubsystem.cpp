#include "MeshRendererSubsystem.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer2.h"
#include "ncengine/graphics/GraphicsUtility.h"

#include "ncengine/debug/Profile.h"

namespace nc::graphics
{
MeshRendererSubsystem::MeshRendererSubsystem(uint32_t maxMeshRenderers, std::span<const MaterialPass::type>)
    : m_transformCache{maxMeshRenderers},
      m_instanceCache{}
{
    MeshRenderer2::s_subsystem = this;
}

auto MeshRendererSubsystem::AddInstance(Entity entity,
                                        MaterialInstanceHandle material,
                                        MaterialPasses passes,
                                        const asset::MeshView& mesh) -> AddInstanceResult
{
    // todo: why give back 'instanceId' - just use entity
    const auto transformIndex = m_transformCache.AddInstance(entity);
    // const auto instanceId = m_instanceCache.AddInstance(transformIndex, material, passes, mesh);
    const auto instanceId = m_instanceCache.StageAdd(transformIndex, material, passes, mesh);
    return AddInstanceResult{transformIndex, instanceId};
}

void MeshRendererSubsystem::RemoveInstance(uint32_t transformIndex,
                                           uint32_t instance,
                                           uint64_t meshId,
                                           MaterialPasses passes)
{
    m_transformCache.RemoveInstance(transformIndex);
    // m_instanceCache.RemoveInstance(instance, meshId, passes);
    m_instanceCache.RemoveInstance(instance, meshId, passes);
}

void MeshRendererSubsystem::SetInstanceMesh(Entity entity, MaterialPasses passes, const asset::MeshView& mesh)
{
    (void)entity;
    (void)passes;
    (void)mesh;
    // m_passCache.UpdateTargetMesh(passes, entity.Index(), mesh);
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

    // m_instanceCache.UpdateMatrices(ecs);
    // return MeshRendererRenderState{
    //     .instanceData = m_instanceCache.BuildState(),
    //     .passData = m_passCache.BuildState()
    // };
}
} // namespace nc::graphics
