#pragma once

#include "BoneCache.h"
#include "SkeletalAnimationRenderState.h"
#include "SkeletalAnimationStorage.h"
#include "SkeletalAnimationTypes.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/Mesh.h"

namespace nc::graphics
{
class SkeletalAnimationSubsystem
{
    public:
        explicit SkeletalAnimationSubsystem(uint32_t maxBones)
            : m_boneCache{maxBones}
        {
        }

        auto GetStorage()          -> SkeletalAnimationStorage& { return m_storage; }
        auto GetBoneCacheStaging() -> BoneCacheStaging&         { return m_boneCache.GetStagingArea(); }

        // Update graph task to process calculate bone transform based on in-flight animations.
        // NOTE: No scheduling requirements - operates entirely on internal or otherwised synchronized data.
        void CalculateBoneMatrices();

        // Update graph task to execute animation state machines and synchronize in-flight animations.
        // IMPORTANT: Must not run concurrently with game logic.
        void UpdateAnimationControllers(ecs::ExplicitEcs<SkinnedMesh> ecs);

        auto BuildState() -> SkeletalAnimationRenderState;
        void OnBeforeSceneLoad();
        void Clear() noexcept;

    private:
        SkeletalAnimationStorage m_storage;

        std::vector<Entity> m_animatedEntities;
        std::vector<gfx2::InFlightAnimation> m_animationState;
        std::vector<Entity> m_completedAnimations;

        BoneCache m_boneCache;

        void Transition(SkinnedMesh& mesh);
        void Start(const MeshInstanceContext& ctx, const AnimationTransition& transition);
        void Stop(const MeshInstanceContext& ctx);
};
} // namespace nc::graphics
