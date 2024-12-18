#include "AnimationStateOrchestrator.h"
#include "SkeletalAnimationStorage.h"

namespace nc::graphics
{
void AnimationStateOrchestrator::Transition(ecs::ComponentPool<SkinnedMesh>& pool,
                                            const SkeletalAnimationStorage& storage)
{
    for (auto& mesh : pool)
    {
        const auto transition = mesh.GetAnimationController().CheckForTransition();
        switch (transition.type)
        {
            case AnimationTransitionType::Continue:
                break;
            case AnimationTransitionType::Loop:
            case AnimationTransitionType::PlayOnce:
                AddAnimation(mesh.GetContext(), transition, storage);
                break;
            case AnimationTransitionType::Stop:
                RemoveAnimation(mesh.GetContext().entity);
                break;
        }
    }
}

void AnimationStateOrchestrator::Remove(std::span<const Entity> toRemove)
{
    for (const auto entity : toRemove)
    {
        RemoveAnimation(entity);
    }
}

void AnimationStateOrchestrator::NotifyCompleted(ecs::ComponentPool<SkinnedMesh>& pool,
                                                 std::span<const Entity> completed)
{
    for (const auto entity : completed)
    {
        pool.Get(entity).GetAnimationController().NotifyCompleteState();
    }
}

void AnimationStateOrchestrator::Purge() noexcept
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

void AnimationStateOrchestrator::AddAnimation(const MeshInstanceContext& ctx,
                                              const nc::AnimationTransition& transition,
                                              const SkeletalAnimationStorage& storage)
{
    // Null animation and boneless mesh are valid, just ignore
    if (transition.toAnimId == NullAnimationId || !storage.HasRig(ctx.meshId))
    {
        return;
    }

    const auto pos = std::ranges::find(m_animatedEntities, ctx.entity);
    if (pos == m_animatedEntities.end())
    {
        m_animatedEntities.push_back(ctx.entity);
        m_animationState.emplace_back(
            ctx.meshId,
            transition.toAnimId,
            transition.fromAnimId,
            ctx.boneDataHandle,
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

void AnimationStateOrchestrator::RemoveAnimation(Entity entity)
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
