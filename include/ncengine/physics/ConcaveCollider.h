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
        auto GetPath() const -> const std::string& { return m_path; }

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
