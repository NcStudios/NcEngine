/**
 * @file SkeletalAnimator.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"
#include "ncengine/graphics/SkeletalAnimationTypes.h"

namespace nc::graphics
{
/** @brief Component that controls skeletal animation for an entity. */
class SkeletalAnimator : public ComponentBase
{
    public:
        static constexpr uint32_t MaxStates = 100u;

        /**
         * @brief Construct a new SkeletalAnimator component.
         * @param entity The parent entity.
         * @param meshUid The UID of the mesh that will be animated by this component.
         * @param animationUid The UID of the asset::SkeletalAnimation that will become the initial animation for this component. The initial state will always loop.
         */
        SkeletalAnimator(Entity entity, std::string meshUid, std::string animationUid);
        SkeletalAnimator(SkeletalAnimator&& other) noexcept  = default;
        SkeletalAnimator(SkeletalAnimator& other) = delete;
        auto operator=(SkeletalAnimator&& other) noexcept -> SkeletalAnimator&  = default;
        auto operator=(SkeletalAnimator& other) -> SkeletalAnimator& = delete;
        ~SkeletalAnimator() noexcept = default;

        /** 
         * @brief Called by SkeletalAnimationSystem. Executes the state machine.
         * All animations whose transition conditions are met emit a signal to be processed.
         * */
        void Execute();

        /** 
         * @brief Add an anim::Loop state to the state machine.
         * @param loopProperties Properties required to create an anim::Loop state.
         */
        auto AddState(anim::Loop loopProperties) -> uint32_t;

        /** 
         * @brief Add an anim::PlayOnce state to the state machine.
         * @param playOnceProperties Properties required to create an anim::PlayOnce state.
         */
        auto AddState(anim::PlayOnce playOnceProperties) -> uint32_t;

        /** 
         * @brief Add an anim::Stop state to the state machine.
         * @param stopProperties Properties required to create an anim::Stop state.
         */
        auto AddState(anim::Stop stopProperties) -> uint32_t;

        /**
         * @brief Interrupt the state machine with an immediate looping animation.
         * Transitions back into the state machine when exitWhen is met.
         * @param animUid The animation to play.
         * @param exitWhen Exit back to the state machine when this condition is met.
         * @param exitTo Exit back to this state.
         */
        void LoopImmediate(std::string animUid, std::function<bool ()> exitWhen, uint32_t exitTo = anim::RootState);

        /**
         * @brief Interrupt the state machine with an immediate animation that plays once.
         * Transitions back into the state machine when the animation has completed.
         * @param animUid The animation to play
         * @param exitTo Exit back to this state.
         */
        void PlayOnceImmediate(std::string animUid, uint32_t exitTo = anim::RootState);

        /**
         * @brief Interrupt the state machine and stop the currently playing animation.
         * Transitions back into the state machine when exitWhen is met.
         * @param exitWhen Exit back to the state machine when this condition is met.
         * @param exitTo Exit back to this state.
         */
        void StopImmediate(std::function<bool ()> exitWhen, uint32_t exitTo = anim::RootState);

        /** @brief Connect to SkeletalAnimationSystem. **/
        auto Connect() noexcept -> Signal<const anim::StateChange&>& { return m_onStateChanged; }

        /** @brief Get the UID of the mesh that will be animated **/
        auto MeshUid() const noexcept -> const std::string& { return m_meshUid; }

        /** @brief Used to manage the play once state **/
        void CompleteFirstRun();

    private:
        auto ExecuteRootState() -> bool;
        auto ExecuteImmediateState() -> bool;
        auto ExecuteExitState() -> bool;
        auto ExecuteChildEnterState(uint32_t childStateHandle) -> bool;
        auto AddState(anim::StopImmediate stopImmediateProperties) -> uint32_t;

        anim::StableSet m_states;
        Signal<const anim::StateChange&> m_onStateChanged;
        std::string m_meshUid;
        std::unique_ptr<anim::State> m_immediateState;
        uint32_t m_activeState;
        bool m_immediateStateSet;
        bool m_rootStateSet;
};

constexpr auto animatorSize = sizeof(SkeletalAnimator);
} // namespace nc::graphics

namespace nc
{
template<>
struct StoragePolicy<graphics::SkeletalAnimator> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = true;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc
