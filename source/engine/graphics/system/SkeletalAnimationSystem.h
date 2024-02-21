#pragma once

#include "ecs/Registry.h"
#include "asset/AssetData.h"
#include "graphics/shader_resource/SkeletalAnimationData.h"
#include "graphics/SkeletalAnimator.h"
#include "graphics/SkeletalAnimationTypes.h"

namespace nc::graphics
{
struct SkeletalAnimationSystemState
{
    // Index consumed by shader that signifies first animation matrix for the given animation
    std::unordered_map<Entity::index_type, uint32_t> animationIndices;
};

class SkeletalAnimationSystem : public StableAddress
{
    public:
        SkeletalAnimationSystem(Registry* registry,
                                Signal<const asset::SkeletalAnimationUpdateEventData&>& onSkeletalAnimationUpdate,
                                Signal<const asset::BoneUpdateEventData&>& onBonesUpdate,
                                Signal<const std::vector<SkeletalAnimationData>&>&& gpuBackendChannel);
        
        auto Execute() -> SkeletalAnimationSystemState;
        void Start(const anim::StateChange& stateChange);
        void Stop(const anim::StateChange& stateChange);
        void Clear() noexcept;

    private:
        // Asset signals
        void UpdateSkeletalAnimationStorage(const asset::SkeletalAnimationUpdateEventData& eventData);
        void UpdateBonesStorage(const asset::BoneUpdateEventData& eventData);

        // Component methods
        void OnStateChanged(const anim::StateChange& data);
        void Add(SkeletalAnimator& animator);
        void Remove(Entity entity);

        Registry* m_registry;

        // Mesh assets
        std::unordered_map<std::string, anim::PackedRig> m_rigs;
        nc::Connection<const asset::BoneUpdateEventData&> m_onBonesUpdate;

        // Animation assets
        std::unordered_map<std::string, asset::SkeletalAnimation> m_animationAssets;
        nc::Connection<const asset::SkeletalAnimationUpdateEventData&> m_onSkeletalAnimationUpdate;

        // Component registration
        Connection<graphics::SkeletalAnimator&> m_onAddConnection;
        Connection<Entity> m_onRemoveConnection;
        std::vector<Connection<const anim::StateChange&>> m_onStateChangedHandlers;
        std::vector<Entity::index_type> m_handlerIndices;

        // Animation data sandbox
        std::vector<Entity> m_unitEntities;
        std::vector<anim::UnitOfWork> m_units;

        // GPU
        Signal<const std::vector<SkeletalAnimationData>&> m_gpuBackendChannel;
};
} // namespace nc::graphics
