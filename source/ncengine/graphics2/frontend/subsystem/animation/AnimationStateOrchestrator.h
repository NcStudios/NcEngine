#pragma once

#include "ncengine/ecs/ComponentPool.h"
#include "ncengine/graphics/Mesh.h"

#include <span>

namespace nc::graphics
{
class SkeletalAnimationStorage;

struct InFlightAnimation
{
    uint64_t meshId = std::numeric_limits<uint32_t>::max();
    uint64_t animId =  std::numeric_limits<uint32_t>::max();
    uint64_t blendFromAnimId =  std::numeric_limits<uint32_t>::max();
    uint32_t dataHandle =  std::numeric_limits<uint32_t>::max();
    float time = 0.0f;
    float blendFromTime = 0.0f;
    float currentTransitionTime = 0.0f;
    float transitionDuration = 0.0f;
    float blendFactor = 0.0f;
};

// T is any component that has a GetSkeletalAnimationController() method and a GetMeshContext() method.
template<typename T>
concept AnimatableComponent = 
requires(T t)
{
    {t.GetSkeletalAnimationController() } -> std::same_as<AnimationController&>;
    {t.GetMeshContext()} -> std::same_as<MeshInstanceContext&>;
};

template<AnimatableComponent T>
class AnimationStateOrchestrator
{
    using StorageType = std::conditional_t<std::is_same_v<T, SkinnedMesh>, SkeletalAnimationStorage, std::monostate>; // If T is a SkinnedMesh, StorageType is SkeletalAnimationStorage. Else, it's a nullptr/monostate (Empty variant option).

    public:
        void Transition(ecs::ComponentPool<T>& pool, const StorageType* storage);
        void NotifyCompleted(ecs::ComponentPool<T>& pool, std::span<const Entity> completed);
        void Remove(std::span<const Entity> toRemove);
        void Purge() noexcept;

        auto GetEntities()   -> std::vector<Entity>&            { return m_animatedEntities; }
        auto GetAnimations() -> std::vector<InFlightAnimation>& { return m_animationState;   }

    private:
        std::vector<Entity> m_animatedEntities;
        std::vector<InFlightAnimation> m_animationState;

        void AddAnimation(const MeshInstanceContext& ctx,
                          const AnimationTransition& transition,
                          const StorageType* storage);
        void RemoveAnimation(Entity entity);
};

template<AnimatableComponent T>
void AnimationStateOrchestrator<T>::Transition(ecs::ComponentPool<T>& pool, const StorageType* storage)
{
    for (auto& animatable : pool)
    {
        const auto transition = animatable.GetSkeletalAnimationController().CheckForTransition();
        switch (transition.type)
        {
            case AnimationTransitionType::Continue:
                break;
            case AnimationTransitionType::Loop:
            case AnimationTransitionType::PlayOnce:
                AddAnimation(animatable.GetMeshContext(), transition, storage);
                break;
            case AnimationTransitionType::Stop:
                RemoveAnimation(animatable.GetMeshContext().entity);
                break;
        }
    }
}

template<AnimatableComponent T>
void AnimationStateOrchestrator<T>::NotifyCompleted(ecs::ComponentPool<T>& pool,
                                                 std::span<const Entity> completed)
{
    for (const auto entity : completed)
    {
        pool.Get(entity).GetSkeletalAnimationController().NotifyCompleteState();
    }
}

template<AnimatableComponent T>
void AnimationStateOrchestrator<T>::Remove(std::span<const Entity> toRemove)
{
    for (const auto entity : toRemove)
    {
        RemoveAnimation(entity);
    }
}

template<AnimatableComponent T>
void AnimationStateOrchestrator<T>::Purge() noexcept
{
    auto newEnd = 0ull;
    for (auto [entity, state] : std::views::zip(m_animatedEntities, m_animationState))
    {
        if (entity.IsPersistent())
        {
            m_animatedEntities[newEnd] = entity;
            m_animationState[newEnd] = state;
            ++newEnd;
        }
    }

    m_animatedEntities.resize(newEnd);
    m_animatedEntities.shrink_to_fit();
    m_animationState.resize(newEnd);
    m_animationState.shrink_to_fit();
}

template<AnimatableComponent T>
void AnimationStateOrchestrator<T>::AddAnimation(const MeshInstanceContext& ctx,
                                                 const nc::AnimationTransition& transition,
                                                 const StorageType* storage)
{
    // Null animation, mesh with no shape keys (and no bones) are valid, just ignore
    if (transition.toAnimId == asset::NullAssetId || (storage != nullptr && !storage->HasRig(ctx.meshId)))
    {
        return;
    }

    uint32_t dataHandle = storage == nullptr ? ctx.shapeKeyDataHandle : ctx.boneDataHandle;

    const auto pos = std::ranges::find(m_animatedEntities, ctx.entity);
    if (pos == m_animatedEntities.end())
    {
        m_animatedEntities.push_back(ctx.entity);
        m_animationState.emplace_back(
            ctx.meshId,
            transition.toAnimId,
            transition.fromAnimId,
            dataHandle,
            0.0f,
            0.0f,
            0.0f,
            transition.transitionDuration,
            0.0f
        );

        return;
    }

    auto& state = m_animationState.at(std::distance(m_animatedEntities.begin(), pos));
    const auto blendFromTime = state.time;
    state = InFlightAnimation{
        ctx.meshId,
        transition.toAnimId,
        transition.fromAnimId,
        ctx.boneDataHandle,
        0.0f,
        blendFromTime,
        0.0f,
        transition.transitionDuration,
        0.0f
    };
}

template<AnimatableComponent T>
void AnimationStateOrchestrator<T>::RemoveAnimation(Entity entity)
{
    auto pos = std::ranges::find(m_animatedEntities, entity);
    if (pos == m_animatedEntities.end())
    {
        return;
    }

    const auto index = std::distance(m_animatedEntities.begin(), pos);
    *pos = m_animatedEntities.back();
    m_animatedEntities.pop_back();
    m_animationState.at(index) = m_animationState.back();
    m_animationState.pop_back();
}

} // namespace nc::graphics
