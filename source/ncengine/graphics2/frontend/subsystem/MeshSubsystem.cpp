#include "MeshSubsystem.h"
#include "animation/SkeletalAnimationStorage.h"
#include "ncengine/Events.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/Mesh.h"
#include "ncengine/graphics/GraphicsUtility.h"
#include "asset/AssetService.h"

#include "ncengine/debug/Profile.h"

namespace
{
auto MakeStaticInstanceData(const nc::MeshInstanceContext& ctx,
                            const nc::MaterialInstance& material) -> nc::graphics::StaticMeshInstanceData
{
    return nc::graphics::StaticMeshInstanceData{
        ctx.transformDataHandle,
        material.GetHandle()
    };
}

auto MakeSkinnedInstanceData(const nc::MeshInstanceContext& ctx,
                             const nc::MaterialInstance& material) -> nc::graphics::SkinnedMeshInstanceData
{
    return nc::graphics::SkinnedMeshInstanceData{
        ctx.transformDataHandle,
        material.GetHandle(),
        ctx.boneDataHandle
    };
}
} // anonymous namespace

namespace nc::graphics
{
MeshSubsystem::MeshSubsystem(SkeletalAnimationStorage& animationStorage,
                             SystemEvents& events,
                             uint32_t maxEntities,
                             uint32_t maxMeshRenderers,
                             uint32_t initialBatchSize)
    : m_transformCache{maxMeshRenderers},
      m_boneCache{maxMeshRenderers * 100u}, // todo: just a random guess
      m_staticMeshInstanceCache{maxEntities, initialBatchSize},
      m_skinnedMeshInstanceCache{maxEntities, initialBatchSize},
      m_animationStorage{&animationStorage},
      m_rebuildStaticsConnection{events.rebuildStatics.Connect(this, &MeshSubsystem::OnRebuildStatics)}
{
    MeshBase::RegisterSubsystem(this);
}

auto MeshSubsystem::GetRigBoneCount(uint64_t meshId) -> uint32_t
{
    const auto _ = m_animationStorage->AcquireReadLock();
    return m_animationStorage->HasRig(meshId)
        ? static_cast<uint32_t>(m_animationStorage->GetRig(meshId).vertexToBone.size())
        : 0u;
}

void MeshSubsystem::AddInstance(MeshInstanceContext& ctx,
                                const MaterialInstance& material,
                                const asset::MeshView& mesh)
{
    ctx.transformDataHandle = m_transformCache.AddInstance(ctx.entity);
    auto addToCache = [&ctx, &material, &mesh](const auto& instanceData, auto& cache) {
        cache.GetStagingArea().AddInstance(
            ctx.entity.Index(),
            material.GetPasses(),
            mesh,
            instanceData
        );
    };

    switch (ctx.type)
    {
        case MeshInstanceType::Static:
        {
            addToCache(MakeStaticInstanceData(ctx, material), m_staticMeshInstanceCache);
            break;
        }
        case MeshInstanceType::Skinned:
        {
            const auto boneCount = GetRigBoneCount(mesh.id);
            ctx.boneDataHandle = boneCount > 0
                ? m_boneCache.Allocate(boneCount)
                : NullBoneCacheHandle;

            addToCache(MakeSkinnedInstanceData(ctx, material), m_skinnedMeshInstanceCache);
            break;
        }
    }
}

void MeshSubsystem::RemoveInstance(const MeshInstanceContext& ctx,
                                   const MaterialInstance& material)
{
    m_transformCache.RemoveInstance(ctx.transformDataHandle);
    auto removeFromCache = [&ctx, &material](auto& cache) {
        cache.GetStagingArea().RemoveInstance(
            ctx.entity.Index(),
            material.GetPasses(),
            ctx.meshId
        );
    };

    switch (ctx.type)
    {
        case MeshInstanceType::Static:
        {
            removeFromCache(m_staticMeshInstanceCache);
            break;
        }
        case MeshInstanceType::Skinned:
        {
            m_boneCache.Free(ctx.boneDataHandle);
            removeFromCache(m_skinnedMeshInstanceCache);
            break;
        }
    }
}

void MeshSubsystem::SetInstanceMesh(MeshInstanceContext& ctx,
                                    const MaterialInstance& material,
                                    const asset::MeshView& newMesh)
{
    auto updateInstance = [&ctx, &material, &newMesh](const auto& instanceData, auto& cache) {
        const auto passes = material.GetPasses();
        cache.GetStagingArea().UpdateInstance(
            ctx.entity.Index(),
            passes,
            passes,
            ctx.meshId,
            newMesh,
            instanceData
        );
    };

    switch (ctx.type)
    {
        case MeshInstanceType::Static:
        {
            updateInstance(MakeStaticInstanceData(ctx, material), m_staticMeshInstanceCache);
            break;
        }
        case MeshInstanceType::Skinned:
        {
            if (ctx.boneDataHandle != NullBoneCacheHandle)
            {
                m_boneCache.Free(ctx.boneDataHandle);
            }

            const auto boneCount = GetRigBoneCount(newMesh.id);
            ctx.boneDataHandle = boneCount > 0
                ? m_boneCache.Allocate(boneCount)
                : NullBoneCacheHandle;

            updateInstance(MakeSkinnedInstanceData(ctx, material), m_skinnedMeshInstanceCache);
            break;
        }
    }
}

void MeshSubsystem::SetInstanceMaterial(const MeshInstanceContext& ctx,
                                        const MaterialInstance& material,
                                        MaterialPasses oldPasses)
{
    const auto meshService = asset::AssetService<asset::MeshView>::Get();
    const auto meshPath = std::string{meshService->GetPath(ctx.meshId)};
    const auto meshView = meshService->Acquire(meshPath);

    auto updateInstance = [&ctx, &material, &meshView, oldPasses](const auto& instanceData, auto& cache) {
        cache.GetStagingArea().UpdateInstance(
            ctx.entity.Index(),
            oldPasses,
            material.GetPasses(),
            ctx.meshId,
            meshView,
            instanceData
        );
    };

    switch (ctx.type)
    {
        case MeshInstanceType::Static:
        {
            updateInstance(MakeStaticInstanceData(ctx, material), m_staticMeshInstanceCache);
            break;
        }
        case MeshInstanceType::Skinned:
        {
            updateInstance(MakeSkinnedInstanceData(ctx, material), m_skinnedMeshInstanceCache);
            break;
        }
    }
}

auto MeshSubsystem::BuildState(ecs::ExplicitEcs<Transform> ecs) -> MeshRenderState
{
    NC_PROFILE_SCOPE("MeshSubsystem::BuildState()", ProfileCategory::Rendering);
    m_transformCache.CommitPendingChanges();
    m_transformCache.UpdateMatrices(ecs);
    m_staticMeshInstanceCache.CommitPendingChanges();
    m_skinnedMeshInstanceCache.CommitPendingChanges();
    return MeshRenderState{
        .transformData = m_transformCache.BuildState(),
        .staticMeshInstanceData = m_staticMeshInstanceCache.BuildState(),
        .skinnedMeshInstanceData = m_skinnedMeshInstanceCache.BuildState(),
        .staticMeshBatches = m_staticMeshInstanceCache.BuildBatches(GetImplementedMaterialPassFlags()),
        .skinnedMeshBatches = m_skinnedMeshInstanceCache.BuildBatches(GetImplementedMaterialPassFlags())
    };
}

void MeshSubsystem::OnBeforeSceneLoad()
{
    // Call here instead of on Clear() to allow the OnRemove callbacks to fire before purging.
    m_staticMeshInstanceCache.Purge();
    m_skinnedMeshInstanceCache.Purge();
}

void MeshSubsystem::OnRebuildStatics()
{
    m_transformCache.MarkStaticsDirty();
}
} // namespace nc::graphics
