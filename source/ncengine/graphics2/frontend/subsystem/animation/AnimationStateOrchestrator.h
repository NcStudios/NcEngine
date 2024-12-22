#pragma once

#include "ncengine/ecs/ComponentPool.h"
#include "ncengine/graphics/Mesh.h"

#include <span>

namespace nc::graphics
{
class SkeletalAnimationStorage;

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

class AnimationStateOrchestrator
{
    public:
        void Transition(ecs::ComponentPool<SkinnedMesh>& pool, const SkeletalAnimationStorage& storage);
        void NotifyCompleted(ecs::ComponentPool<SkinnedMesh>& pool, std::span<const Entity> completed);
        void Remove(std::span<const Entity> toRemove);
        void Purge() noexcept;

        auto GetEntities()   -> std::vector<Entity>&            { return m_animatedEntities; }
        auto GetAnimations() -> std::vector<InFlightAnimation>& { return m_animationState;   }

    private:
        std::vector<Entity> m_animatedEntities;
        std::vector<InFlightAnimation> m_animationState;

        void AddAnimation(const MeshInstanceContext& ctx,
                          const AnimationTransition& transition,
                          const SkeletalAnimationStorage& storage);
        void RemoveAnimation(Entity entity);
};
} // namespace nc::graphics
