#pragma once

#include "AnimationStateOrchestrator.h"
#include "BoneCache.h"
#include "Rig.h"
#include "SkeletalAnimationRenderState.h"
#include "SkeletalAnimationStorage.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/Mesh.h"

namespace nc::graphics
{
// Animation system interface for consumption by other graphics subsystems.
class ISkeletalAnimationSubsystem
{
    public:
        explicit ISkeletalAnimationSubsystem(uint32_t maxBones)
            : m_boneCache{maxBones}
        {
        }

        auto GetStorage() -> SkeletalAnimationStorage& { return m_storage; }
        auto AllocateBones(uint64_t meshId) -> BoneCacheHandle;
        void NotifyRemove(Entity entity, BoneCacheHandle boneIndex);

    protected:
        SkeletalAnimationStorage m_storage;
        BoneCache m_boneCache;
        std::vector<Entity> m_removed;

        ~ISkeletalAnimationSubsystem() = default;
        auto GetRigBoneCount(uint64_t meshId) -> uint32_t;
};

class SkeletalAnimationSubsystem : public ISkeletalAnimationSubsystem
{
    public:
        explicit SkeletalAnimationSubsystem(uint32_t maxBones)
            : ISkeletalAnimationSubsystem{maxBones}
        {
        }

        // Update graph task
        // IMPORTANT: This must not run concurrently with game logic.
        void Update(ecs::ExplicitEcs<SkinnedMesh> ecs);
        auto BuildState() -> SkeletalAnimationRenderState;
        void OnBeforeSceneLoad();

    private:
        AnimationStateOrchestrator<SkinnedMesh> m_stateOrchestrator;
        std::vector<Entity> m_completedAnimations;

        void CommitPendingChanges();
        void CalculateBoneMatrices();
        void NotifyCompletedAnimations(ecs::ComponentPool<SkinnedMesh>& pool);
};
} // namespace nc::graphics
