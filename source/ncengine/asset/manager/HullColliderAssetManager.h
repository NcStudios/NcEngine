#pragma once

#include "asset/AssetService.h"
#include "utility/StringMap.h"
#include "ncengine/utility/Signal.h"

#include "ncasset/AssetsFwd.h"

#include <string>

namespace nc::asset
{
struct ConvexHullUpdateEventData;

class HullColliderAssetManager : public IAssetService<ConvexHullView, std::string>
{
    public:
        explicit HullColliderAssetManager(const std::string& assetDirectory);

        bool Load(const std::string& path, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const std::string& path, asset_flags_type flags = AssetFlags::None) override;
        void UnloadAll(asset_flags_type flags = AssetFlags::None) override;
        auto Acquire(const std::string& path, asset_flags_type flags = AssetFlags::None) const -> ConvexHullView override;
        auto Acquire(AssetId id, asset_flags_type flags = AssetFlags::None) const -> ConvexHullView override;
        bool IsLoaded(const std::string& path, asset_flags_type flags = AssetFlags::None) const override;
        auto GetPath(AssetId id) const -> std::string_view override { return m_map.at(m_map.index(id)); }
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto GetAssetType() const noexcept -> asset::AssetType override { return asset::AssetType::HullCollider; }
        auto OnUpdate() -> Signal<const ConvexHullUpdateEventData&>& { return m_onUpdate; }

    private:
        StringTable m_map;
        std::string m_assetDirectory;
        Signal<const ConvexHullUpdateEventData&> m_onUpdate;
};
} // namespace nc::asset
