/**
 * @file ConcaveCollider.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"

namespace nc::physics
{
/** @brief Component managing concave collision geometry. */
class ConcaveCollider final : public ComponentBase
{
    public:
        ConcaveCollider(Entity entity, std::string assetPath);

        /** @brief Get the path to the geometry asset. */
        auto GetAssetPath() const noexcept -> const std::string& { return m_path; }

        /**
         * @brief Set the path to the geometry asset.
         * @note Intended for editor use. Updating the underlying geometry requires invoking SystemEvents::rebuildStatics
         *       after updating the asset path.
         */
        auto SetAssetPath(std::string assetPath) { m_path = std::move(assetPath); }

    private:
        std::string m_path;
};
} // namespace nc::physics

namespace nc
{
template<>
struct StoragePolicy<physics::ConcaveCollider> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = true;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc
