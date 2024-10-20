#pragma once

#include "ecs/Registry.h"
#include "asset/AssetData.h"
#include "graphics/shader_resource/ShaderResourceBus.h"
#include "graphics/shader_resource/SkeletalAnimationData.h"
#include "graphics/shader_resource/StorageBufferHandle.h"
#include "graphics/SkeletalAnimator.h"
#include "graphics/SkeletalAnimationTypes.h"

namespace nc::graphics
{
struct SkeletalAnimationSystemState
{
    // Index consumed by shader that signifies first animation matrix for the given animation
    std::unordered_map<Entity::index_type, uint32_t> animationIndices;
};

constexpr uint32_t AvgBonesPerAnim = 10u;

class SkeletalAnimationSystem : public StableAddress
{
    public:
        SkeletalAnimationSystem(Registry* registry,
                                ShaderResourceBus* shaderResourceBus,
                                uint32_t maxSkeletalAnimations,
                                Signal<const asset::SkeletalAnimationUpdateEventData&>& onSkeletalAnimationUpdate,
                                Signal<const asset::BoneUpdateEventData&>& onBonesUpdate);
        
        auto Execute(uint32_t frameIndex) -> SkeletalAnimationSystemState;
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
        nc::Connection m_onBonesUpdate;

        // Animation assets
        std::unordered_map<std::string, asset::SkeletalAnimation> m_animationAssets;
        nc::Connection m_onSkeletalAnimationUpdate;

        // Component registration
        Connection m_onAddConnection;
        Connection m_onRemoveConnection;
        std::vector<Connection> m_onStateChangedHandlers;
        std::vector<Entity::index_type> m_handlerIndices;

        // Animation data sandbox
        std::vector<Entity> m_unitEntities;
        std::vector<anim::UnitOfWork> m_units;

        // GPU
        StorageBufferHandle m_skeletalAnimationDataBuffer;
};
} // namespace nc::graphics
