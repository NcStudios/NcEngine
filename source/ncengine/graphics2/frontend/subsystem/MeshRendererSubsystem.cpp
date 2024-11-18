#include "MeshRendererSubsystem.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer2.h"


#include "ncengine/debug/Profile.h"

namespace nc::graphics
{
MeshRendererSubsystem::MeshRendererSubsystem(uint32_t maxMeshRenderers, std::span<const MaterialPass::type> passes)
    : m_instanceCache{maxMeshRenderers},
      m_passCache{passes}
{
    MeshRenderer2::s_subsystem = this;
}

auto MeshRendererSubsystem::AddInstance(Entity entity,
                                        MaterialInstanceHandle material,
                                        MaterialPasses passes,
                                        const asset::MeshView& mesh) -> uint32_t
{
    const auto instance = m_instanceCache.AddInstance(entity, material);
    m_passCache.AddTarget(passes, entity.Index(), instance, mesh);
    return instance;
}

void MeshRendererSubsystem::RemoveInstance(Entity entity,
                                           uint32_t instance,
                                           MaterialPasses passes)
{
    m_instanceCache.RemoveInstance(instance);
    m_passCache.RemoveTarget(passes, entity.Index());
}

void MeshRendererSubsystem::SetInstanceMesh(Entity entity, MaterialPasses passes, const asset::MeshView& mesh)
{
    m_passCache.UpdateTargetMesh(passes, entity.Index(), mesh);
}

auto MeshRendererSubsystem::BuildState(ecs::ExplicitEcs<MeshRenderer2, Transform> ecs) -> MeshRendererRenderState
{
    NC_PROFILE_SCOPE("MeshRendererSubsystem::BuildState()", ProfileCategory::Rendering);
    m_instanceCache.UpdateMatrices(ecs);
    return MeshRendererRenderState{
        .instanceData = m_instanceCache.BuildState(),
        .passData = m_passCache.BuildState()
    };
}
} // namespace nc::graphics
