#pragma once

#include "BoneCache.h"
#include "Rig.h"
#include "SkeletalAnimationRenderState.h"
#include "SkeletalAnimationStorage.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/Mesh.h"

namespace nc::graphics
{
struct InFlightAnimation
{
    uint64_t meshId = 0;
    uint64_t animId = 0;
    uint64_t blendFromAnimId = 0;
    uint32_t boneIndex = 0;
    float time = 0.0f;
    float blendFromTime = 0.0f;
    float currentTransitionTime = 0.0f;
    float transitionDuration = 0.0f;
    float blendFactor = 0.0f;
};

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
        void Clear() noexcept;

    private:
        std::vector<Entity> m_animatedEntities;
        std::vector<InFlightAnimation> m_animationState;
        std::vector<Entity> m_completedAnimations;

        void CommitPendingChanges();
        void CalculateBoneMatrices();
        void NotifyCompletedAnimations(ecs::ComponentPool<SkinnedMesh>& pool);
        void Transition(ecs::ComponentPool<SkinnedMesh>& pool);
        void Start(const MeshInstanceContext& ctx, const AnimationTransition& transition);
        void Stop(const MeshInstanceContext& ctx);
};
} // namespace nc::graphics
