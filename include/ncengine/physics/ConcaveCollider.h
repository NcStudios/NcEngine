#pragma once

#include "ncengine/ecs/Component.h"

namespace nc::physics
{
class ConcaveCollider final : public ComponentBase
{
    NC_ENABLE_IN_EDITOR(ConcaveCollider)

    public:
        static constexpr auto ComponentId = 9ull;

        ConcaveCollider(Entity entity, std::string assetPath);
        auto GetPath() const -> const std::string& { return m_path; }
        void SetAsset(std::string assetPath);

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
