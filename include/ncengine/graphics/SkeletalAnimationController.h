/**
 * @file SkeletalAnimationController.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/utility/SparseMap.h"

#include <functional>
#include <limits>

namespace nc
{
/** @brief Identifier for a null animation asset. */
constexpr auto NullAnimationId = std::numeric_limits<uint64_t>::max();

/** @name Animation State Id Types */
using AnimationStateId = uint32_t;
constexpr auto NullAnimationState = std::numeric_limits<AnimationStateId>::max();
constexpr auto RootAnimationState = AnimationStateId{0u};
constexpr auto ImmediateAnimationState = NullAnimationState - 1u;
constexpr auto MaxAnimationStates = 32ull;

/** @name Animation Transition Types */
using TransitionCondition = std::move_only_function<bool()>;
constexpr bool ConditionNever() { return false; }
constexpr auto UseDefaultTransitionDuration = -1.0f;

/** @brief Animation state that loops until the exit condition is met. */
struct LoopAnimation
{
    uint64_t animId = NullAnimationId;
    TransitionCondition enterWhen = ConditionNever;
    AnimationStateId enterFrom = RootAnimationState;
    TransitionCondition exitWhen = ConditionNever;
    AnimationStateId exitTo = RootAnimationState;
    float transitionDuration = UseDefaultTransitionDuration;
};

/** @brief Animation state that plays once before transitioning to the exitTo state. */
struct PlayOnceAnimation
{
    uint64_t animId = NullAnimationId;
    TransitionCondition enterWhen = ConditionNever;
    AnimationStateId enterFrom = RootAnimationState;
    AnimationStateId exitTo = RootAnimationState;
    float transitionDuration = UseDefaultTransitionDuration;
};

/** @brief Animation state that stops the state machine.. */
struct StopAnimation
{
    TransitionCondition enterWhen = ConditionNever;
    AnimationStateId enterFrom = RootAnimationState;
    float transitionDuration = UseDefaultTransitionDuration;
};

/** @cond internal */
enum class AnimationTransitionType : uint8_t
{
    Continue,
    Loop,
    PlayOnce,
    Stop
};

struct AnimationState
{
    uint64_t animId = NullAnimationId;
    TransitionCondition enterWhen = ConditionNever;
    TransitionCondition exitWhen = ConditionNever;
    AnimationStateId enterFrom = NullAnimationState;
    AnimationStateId exitTo = NullAnimationState;
    float transitionDuration = UseDefaultTransitionDuration;
    AnimationTransitionType action = AnimationTransitionType::Loop;
    std::vector<AnimationStateId> successors = {};
};

struct AnimationTransition
{
    uint64_t toAnimId = NullAnimationId;
    uint64_t fromAnimId = NullAnimationId;
    float transitionDuration = 0.0f;
    AnimationTransitionType type = AnimationTransitionType::Continue;
};
/** @endcond internal */

/** @brief State machine that controls animation transitions for a SkinnedMesh. */
class SkeletalAnimationController
{
    public:
        /**
         * @brief Construct a SkeletalAnimationController.
         * 
         * The controller is initialized with a default state that loops the animation asset specified by animationId.
         * This state is set with the id 'RootState' and cannot not be removed.
         */
        explicit SkeletalAnimationController(uint64_t animationId = NullAnimationId,
                                             float defaultTransitionDuration = 0.3f);

        /**
         * @name State Machine Functions
         * 
         * Animation transitions are managed automatically based on the properties of current animation states.
         */
        auto GetActiveState() const -> AnimationStateId { return m_activeState; }
        auto GetCurrentAnimationId() const -> uint64_t;
        auto AddState(LoopAnimation&& properties) -> AnimationStateId;
        auto AddState(PlayOnceAnimation&& properties) -> AnimationStateId;
        auto AddState(StopAnimation&& properties) -> AnimationStateId;
        void SetAnimation(AnimationStateId stateId, uint64_t animationId);
        auto GetDefaultTransitionDuration() const -> float { return m_defaultTransitionDuration; }
        void SetDefaultTransitionDuration(float dur) { m_defaultTransitionDuration = dur; }

        /**
         * @name Immediate Transition Functions
         * 
         * Immediate transitions interrupt the state machine and move to the specified state. Control is returned to
         * the state machine upon the exit condition being met, or the immediate animation finishing, for PlayOnce.
         */
        void LoopImmediate(uint64_t animId,
                           TransitionCondition&& exitWhen,
                           AnimationStateId exitTo = RootAnimationState,
                           float transitionDuration = UseDefaultTransitionDuration);

        void PlayOnceImmediate(uint64_t animId,
                               AnimationStateId exitTo = RootAnimationState,
                               float transitionDuration = UseDefaultTransitionDuration);

        void StopImmediate(TransitionCondition&& exitWhen,
                           AnimationStateId exitTo = RootAnimationState,
                           float transitionDuration = UseDefaultTransitionDuration);

        /** @cond internal */
        void RefreshAnimation();
        void NotifyCompleteState();
        auto CheckForTransition() -> AnimationTransition;
        /** @endcond internal */

    private:
        sparse_map<AnimationState> m_states;
        std::unique_ptr<AnimationState> m_immediateState;
        AnimationStateId m_activeState = NullAnimationState;
        AnimationStateId m_queuedState = RootAnimationState;
        AnimationStateId m_nextStateId = RootAnimationState + 1;
        float m_defaultTransitionDuration;
        uint64_t m_prevAnimId = NullAnimationId;
        bool m_cycleCompleted = false;

        auto AddStateImpl(AnimationState&& in) -> AnimationStateId;
        auto CalculateTransitionDuration(float requestedDuration) const -> float;
        void QueueImmediateTransition();
        auto TransitionQueuedState() -> AnimationTransition;
        auto TransitionState(AnimationState& from, AnimationState& to, AnimationStateId toId) -> AnimationTransition;
        auto ShouldExitState(AnimationState& current, bool cycleCompleted) -> bool;
};
} // namespace nc
