#pragma once

#include "MeshRenderState.h"
#include "TransformCache.h"
#include "InstanceCache.h"
#include "animation/BoneCache.h"

#include "ncengine/ecs/EcsFwd.h"
#include "ncengine/utility/Signal.h"

namespace nc
{
class MaterialInstance;
struct MeshInstanceContext;
struct SystemEvents;
class Transform;

namespace graphics
{
class SkeletalAnimationStorage;

class MeshSubsystem
{
    public:
        explicit MeshSubsystem(SkeletalAnimationStorage& animationStorage,
                               BoneCacheStaging& boneCacheStaging,
                               SystemEvents& events,
                               uint32_t maxEntities,
                               uint32_t maxMeshRenderers,
                               uint32_t initialBatchSize);

        void AddInstance(MeshInstanceContext& ctx,
                         const MaterialInstance& material,
                         const asset::MeshView& mesh);

        void RemoveInstance(const MeshInstanceContext& ctx,
                            const MaterialInstance& material);

        void SetInstanceMesh(MeshInstanceContext& ctx,
                             const MaterialInstance& material,
                             const asset::MeshView& newMesh);

        void SetInstanceMaterial(const MeshInstanceContext& ctx,
                                 const MaterialInstance& material,
                                 MaterialPasses oldPasses);

        auto BuildState(ecs::ExplicitEcs<Transform> ecs) -> MeshRenderState;
        void OnBeforeSceneLoad();

    private:
        TransformCache m_transformCache;
        InstanceCache<StaticMeshInstanceData> m_staticMeshInstanceCache;
        InstanceCache<SkinnedMeshInstanceData> m_skinnedMeshInstanceCache;
        SkeletalAnimationStorage* m_animationStorage;
        BoneCacheStaging* m_boneCache;
        Connection m_rebuildStaticsConnection;

        auto GetRigBoneCount(uint64_t meshId) -> uint32_t;
        void OnRebuildStatics();
};
} // namespace graphics
} // namespace nc
