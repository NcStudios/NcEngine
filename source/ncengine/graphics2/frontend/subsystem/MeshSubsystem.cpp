#include "MeshSubsystem.h"
#include "ncengine/Events.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer2.h"
#include "ncengine/graphics/GraphicsUtility.h"
#include "asset/AssetService.h"

#include "ncengine/debug/Profile.h"

namespace nc::graphics
{
MeshSubsystem::MeshSubsystem(SystemEvents& events,
                             uint32_t maxEntities,
                             uint32_t maxMeshRenderers,
                             uint32_t initialBatchSize)
    : m_transformCache{maxMeshRenderers},
      m_staticMeshInstanceCache{maxEntities, initialBatchSize},
      m_rebuildStaticsConnection{events.rebuildStatics.Connect(this, &MeshSubsystem::OnRebuildStatics)}
{
    MeshRenderer2::s_subsystem = this;
}

auto MeshSubsystem::AddInstance(Entity entity,
                                MaterialInstanceHandle material,
                                MaterialPasses passes,
                                const asset::MeshView& mesh) -> TransformDataHandle
{
    const auto transformIndex = m_transformCache.AddInstance(entity);
    m_staticMeshInstanceCache.GetStagingArea().AddInstance(
        entity.Index(),
        passes,
        mesh,
        StaticMeshInstanceData{transformIndex, material}
    );

    return transformIndex;
}

void MeshSubsystem::RemoveInstance(Entity entity,
                                   uint32_t transformIndex,
                                   uint64_t meshId,
                                   MaterialPasses passes)
{
    m_transformCache.RemoveInstance(transformIndex);
    m_staticMeshInstanceCache.GetStagingArea().RemoveInstance(entity.Index(), passes, meshId);
}

void MeshSubsystem::SetInstanceMesh(Entity entity,
                                    uint32_t transformIndex,
                                    MaterialInstanceHandle materialIndex,
                                    MaterialPasses passes,
                                    uint64_t oldMeshId,
                                    const asset::MeshView& newMesh)
{
    m_staticMeshInstanceCache.GetStagingArea().UpdateInstance(
        entity.Index(),
        passes,
        passes,
        oldMeshId,
        newMesh,
        StaticMeshInstanceData{
            transformIndex,
            materialIndex
        }
    );
}

void MeshSubsystem::SetInstanceMaterial(Entity entity,
                                        uint32_t transformIndex,
                                        MaterialInstanceHandle materialIndex,
                                        MaterialPasses oldPasses,
                                        MaterialPasses newPasses,
                                        uint64_t meshId)
{
    const auto meshService = asset::AssetService<asset::MeshView>::Get();
    const auto meshPath = std::string{meshService->GetPath(meshId)};
    const auto meshView = meshService->Acquire(meshPath);
    m_staticMeshInstanceCache.GetStagingArea().UpdateInstance(
        entity.Index(),
        oldPasses,
        newPasses,
        meshId,
        meshView,
        StaticMeshInstanceData{
            transformIndex,
            materialIndex
        }
    );
}

auto MeshSubsystem::BuildState(ecs::ExplicitEcs<MeshRenderer2, Transform> ecs) -> MeshRenderState
{
    NC_PROFILE_SCOPE("MeshSubsystem::BuildState()", ProfileCategory::Rendering);
    m_transformCache.CommitPendingChanges();
    m_transformCache.UpdateMatrices(ecs);
    m_staticMeshInstanceCache.CommitPendingChanges();
    return MeshRenderState{
        .transformData = m_transformCache.BuildState(),
        .staticMeshInstanceData = m_staticMeshInstanceCache.BuildState(),
        .staticMeshBatches = m_staticMeshInstanceCache.BuildBatches(GetImplementedMaterialPassFlags())
    };
}

void MeshSubsystem::OnBeforeSceneLoad()
{
    // Call here instead of on Clear() to allow the OnRemove callbacks to fire before purging.
    m_staticMeshInstanceCache.Purge();
}

void MeshSubsystem::OnRebuildStatics()
{
    m_transformCache.MarkStaticsDirty();
}
} // namespace nc::graphics
