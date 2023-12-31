#pragma once

#include "ncengine/assets/Assets.h"
#include "ncengine/ecs/Component.h"

#include <memory>

namespace nc::graphics
{
class SkeletalAnimator : public ComponentBase
{
    public:
        SkeletalAnimator(Entity entity, std::string assetUid);
        SkeletalAnimator(SkeletalAnimator&& other) noexcept;
        SkeletalAnimator(SkeletalAnimator& other) = delete;
        auto operator=(SkeletalAnimator&& other) noexcept -> SkeletalAnimator&;
        auto operator=(SkeletalAnimator& other) -> SkeletalAnimator& = delete;
        ~SkeletalAnimator() noexcept;

        // Main
        void Execute();

        // Connect to system
        auto Connect() -> Signal<const anim::PlayState&>& { return m_onPlayStateChanged; };

        // Get uid of mesh that will be animated
        auto MeshUid() -> const std::string& { return m_meshUid; }

    private:
        std::string m_meshUid;
        Signal<const anim::PlayState&> m_onPlayStateChanged;
}
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
