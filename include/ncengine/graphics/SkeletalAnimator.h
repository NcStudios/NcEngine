/**
 * @file SkeletalAnimationTypes.h
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

        /** @brief Called by SkeletalAnimationSystem. Executes the state machine; all transitioning animations emit a signal to be processed. */
        void Execute();

        /** @brief Connect to SkeletalAnimationSystem. **/
        auto Connect() const noexcept -> Signal<const anim::PlayState&>& { return m_onPlayStateChanged; };

        /** @brief Get UID of mesh that will be animated **/
        auto MeshUid() const noexcept -> const std::string& { return m_meshUid; }

        /** @brief Used to manage the play once state **/
        void CompleteFirstRun();

    private:
        auto ExecuteInitialState() -> bool;

        std::string m_meshUid;
        Signal<const anim::PlayState&> m_onPlayStateChanged;
        anim::State m_initialState;
        bool m_initialStateInitialized;
};
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
