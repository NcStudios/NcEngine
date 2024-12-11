#include "ncengine/graphics/SkeletalAnimationController.h"

#include "ncutility/ScopeExit.h"

namespace nc
{
SkeletalAnimationController::SkeletalAnimationController(uint64_t animationId,
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

void SkeletalAnimationController::SetRootAnimation(uint64_t animationId)
{
    // todo: test this
    m_states.at(RootAnimationState).animId = animationId;
    if (m_activeState == RootAnimationState)
    {
        m_activeState = NullAnimationState;
        m_queuedState = RootAnimationState;
    }
}

void SkeletalAnimationController::RemoveState(AnimationStateId stateId)
{
    // todo: test this
    if (m_activeState == stateId)
    {
        m_activeState = NullAnimationState;
        m_queuedState = RootAnimationState;
    }

    m_states.erase(stateId);
}

void SkeletalAnimationController::LoopImmediate(uint64_t animId,
                                                TransitionCondition&& exitWhen,
                                                AnimationStateId exitTo,
                                                float transitionDuration)
{
    m_queuedState = ImmediateAnimationState;
    m_immediateState = std::make_unique<AnimationState>(AnimationState{
        .animId = animId,
        .enterWhen = std::move(exitWhen),
        .exitTo = exitTo,
        .transitionDuration = transitionDuration,
        .action = AnimationTransitionType::Loop
    });
}

void SkeletalAnimationController::PlayOnceImmediate(uint64_t animId,
                                                    AnimationStateId exitTo,
                                                    float transitionDuration)
{
    m_queuedState = ImmediateAnimationState;
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
    m_queuedState = ImmediateAnimationState;
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
    else if (m_activeState == NullAnimationState)
    {
        return AnimationTransition{};
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

auto SkeletalAnimationController::GetCurrentAnimationId() const -> uint64_t
{
    switch (m_activeState)
    {
        case ImmediateAnimationState:
            return m_queuedState == NullAnimationId
                ? m_immediateState->animId
                : m_prevImmediateAnimId;
        case NullAnimationState:
            return NullAnimationId;
        default:
            return m_states.at(m_activeState).animId;
    }
}

auto SkeletalAnimationController::CalculateTransitionDuration(float requestedDuration) const -> float
{
    return requestedDuration == UseDefaultTransitionDuration
        ? m_defaultTransitionDuration
        : requestedDuration;
}

auto SkeletalAnimationController::TransitionQueuedState() -> AnimationTransition
{
    const auto curAnim = GetCurrentAnimationId();
    m_prevImmediateAnimId = NullAnimationId;
    m_activeState = std::exchange(m_queuedState, NullAnimationState);
    switch (m_activeState)
    {
        case ImmediateAnimationState:
            return AnimationTransition{
                m_immediateState->animId,
                curAnim,
                CalculateTransitionDuration(m_immediateState->transitionDuration),
                m_immediateState->action
            };
        case RootAnimationState:
            return AnimationTransition{
                m_states.at(RootAnimationState).animId,
                curAnim,
                m_defaultTransitionDuration,
                AnimationTransitionType::Loop
            };
        default:
            // this is used to recreate animation state when mesh has changed
            auto& state = m_states.at(m_activeState);
            return AnimationTransition{
                state.animId,
                state.animId,
                CalculateTransitionDuration(state.transitionDuration),
                state.action
            };
            // NC_ASSERT(false, "Unexpected animation state queued");
            // std::unreachable();
    }
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
