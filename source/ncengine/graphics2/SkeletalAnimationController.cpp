#include "ncengine/graphics/SkeletalAnimationController.h"

#include "ncutility/ScopeExit.h"

#include <utility>

namespace nc
{
SkeletalAnimationController::SkeletalAnimationController(asset::AssetId animationId,
                                                         float defaultTransitionDuration)
    : m_states{4ull, MaxAnimationStates},
      m_defaultTransitionDuration{defaultTransitionDuration}
{
    m_states.emplace(
        RootAnimationState,
        AnimationState{
            .animId = animationId,
            .action = AnimationTransitionType::Loop
        }
    );
}

auto SkeletalAnimationController::GetCurrentAnimationId() const -> asset::AssetId
{
    // Id is cached here if queued transition overwrote the current id (e.g. immediate -> immediate)
    if (m_prevAnimId != asset::NullAssetId)
    {
        return m_prevAnimId;
    }

    switch (m_activeState)
    {
        case ImmediateAnimationState:
            return m_immediateState->animId;
        case NullAnimationState:
            return asset::NullAssetId;
        default:
            return m_states.at(m_activeState).animId;
    }
}

auto SkeletalAnimationController::AddState(LoopAnimation&& properties) -> AnimationStateId
{
    return AddStateImpl(AnimationState{
        .animId = properties.animId,
        .enterWhen = std::move(properties.enterWhen),
        .exitWhen = std::move(properties.exitWhen),
        .enterFrom = properties.enterFrom,
        .exitTo = properties.exitTo,
        .transitionDuration = properties.transitionDuration,
        .action = AnimationTransitionType::Loop
    });
}

auto SkeletalAnimationController::AddState(PlayOnceAnimation&& properties) -> AnimationStateId
{
    return AddStateImpl(AnimationState{
        .animId = properties.animId,
        .enterWhen = std::move(properties.enterWhen),
        .enterFrom = properties.enterFrom,
        .exitTo = properties.exitTo,
        .transitionDuration = properties.transitionDuration,
        .action = AnimationTransitionType::PlayOnce
    });
}

auto SkeletalAnimationController::AddState(StopAnimation&& properties) -> AnimationStateId
{
    return AddStateImpl(AnimationState{
        .enterWhen = std::move(properties.enterWhen),
        .enterFrom = properties.enterFrom,
        .transitionDuration = properties.transitionDuration,
        .action = AnimationTransitionType::Stop
    });
}

auto SkeletalAnimationController::GetAnimation(AnimationStateId stateId) const -> asset::AssetId
{
    return m_states.at(stateId).animId;
}

void SkeletalAnimationController::SetAnimation(AnimationStateId stateId, asset::AssetId animationId)
{
    const auto oldId = std::exchange(m_states.at(stateId).animId, animationId);
    if (m_activeState == stateId)
    {
        m_queuedState = stateId;
        m_prevAnimId = oldId;
    }
}

void SkeletalAnimationController::LoopImmediate(asset::AssetId animId,
                                                TransitionCondition&& exitWhen,
                                                AnimationStateId exitTo,
                                                float transitionDuration)
{
    QueueImmediateTransition();
    m_immediateState = std::make_unique<AnimationState>(AnimationState{
        .animId = animId,
        .exitWhen = std::move(exitWhen),
        .exitTo = exitTo,
        .transitionDuration = transitionDuration,
        .action = AnimationTransitionType::Loop
    });
}

void SkeletalAnimationController::PlayOnceImmediate(asset::AssetId animId,
                                                    AnimationStateId exitTo,
                                                    float transitionDuration)
{
    QueueImmediateTransition();
    m_immediateState = std::make_unique<AnimationState>(AnimationState{
        .animId = animId,
        .exitTo = exitTo,
        .transitionDuration = transitionDuration,
        .action = AnimationTransitionType::PlayOnce
    });
}

void SkeletalAnimationController::StopImmediate(TransitionCondition&& exitWhen,
                                                AnimationStateId exitTo,
                                                float transitionDuration)
{
    QueueImmediateTransition();
    m_immediateState = std::make_unique<AnimationState>(AnimationState{
        .exitWhen = std::move(exitWhen),
        .exitTo = exitTo,
        .transitionDuration = transitionDuration,
        .action = AnimationTransitionType::Stop
    });
}

auto SkeletalAnimationController::CheckForTransition() -> AnimationTransition
{
    const auto cycleCompleted = std::exchange(m_cycleCompleted, false);

    if (m_queuedState != NullAnimationState)
    {
        return TransitionQueuedState();
    }
    else if (m_immediateState)
    {
        if (ShouldExitState(*m_immediateState, cycleCompleted))
        {
            SCOPE_EXIT(m_immediateState.release();)
            const auto exitTo = m_immediateState->exitTo;
            return TransitionState(*m_immediateState, m_states.at(exitTo), exitTo);
        }

        return AnimationTransition{};
    }

    auto& curState = m_states.at(m_activeState);
    if (ShouldExitState(curState, cycleCompleted))
    {
        const auto exitTo = curState.exitTo;
        return TransitionState(curState, m_states.at(exitTo), exitTo);
    }

    for (const auto successorId : curState.successors)
    {
        auto& successor = m_states.at(successorId);
        if (successor.enterWhen())
        {
            return TransitionState(curState, successor, successorId);
        }
    }

    return AnimationTransition{};
}

void SkeletalAnimationController::RefreshAnimation()
{
    // Redundant transition to force subsystem to rebuild state. Useful if a mesh is updated.
    m_queuedState = m_activeState;
}

void SkeletalAnimationController::NotifyCompleteState()
{
    m_cycleCompleted = true;
}

auto SkeletalAnimationController::AddStateImpl(AnimationState&& in) -> AnimationStateId
{
    const auto id = m_nextStateId++;
    m_states.at(in.enterFrom).successors.push_back(id);
    m_states.emplace(id, std::move(in));
    return id;
}

auto SkeletalAnimationController::CalculateTransitionDuration(float requestedDuration) const -> float
{
    return requestedDuration == UseDefaultTransitionDuration
        ? m_defaultTransitionDuration
        : requestedDuration;
}

void SkeletalAnimationController::QueueImmediateTransition()
{
    m_queuedState = ImmediateAnimationState;
    if (m_immediateState)
    {
        m_prevAnimId = m_immediateState->animId;
    }
}

auto SkeletalAnimationController::TransitionQueuedState() -> AnimationTransition
{
    const auto curAnim = GetCurrentAnimationId();
    m_prevAnimId = asset::NullAssetId;
    m_activeState = std::exchange(m_queuedState, NullAnimationState);
    auto& state = m_activeState == ImmediateAnimationState
        ? *m_immediateState
        : m_states.at(m_activeState);

    return AnimationTransition{
        state.animId,
        curAnim,
        CalculateTransitionDuration(state.transitionDuration),
        state.action
    };
}

auto SkeletalAnimationController::TransitionState(AnimationState& from,
                                                  AnimationState& to,
                                                  AnimationStateId toId) -> AnimationTransition
{
    m_activeState = toId;
    return AnimationTransition{
        to.animId,
        from.animId,
        CalculateTransitionDuration(to.transitionDuration),
        to.action
    };
}

auto SkeletalAnimationController::ShouldExitState(AnimationState& current, bool cycleCompleted) -> bool
{
    const auto playOnceComplete = cycleCompleted && current.action == AnimationTransitionType::PlayOnce;
    return playOnceComplete || current.exitWhen();
}
} // namespace nc
