#pragma once

#include "asset/AssetService.h"
#include "utility/StringMap.h"
#include "ncengine/utility/Signal.h"

#include "ncasset/AssetsFwd.h"

#include <string>

namespace nc::asset
{
struct MeshColliderUpdateEventData;

class MeshColliderAssetManager : public IAssetService<MeshColliderView, std::string>
{
    public:
        explicit MeshColliderAssetManager(const std::string& meshColliderAssetDirectory);

        bool Load(const std::string& path, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const std::string> paths, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const std::string& path) override;
        void UnloadAll() override;
        auto Acquire(const std::string& path) const -> MeshColliderView override;
        auto Acquire(AssetId id) const -> MeshColliderView override;
        bool IsLoaded(const std::string& path) const override;
        auto GetPath(AssetId id) const -> std::string_view override { return m_map.at(m_map.index(id)); }
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto GetAssetType() const noexcept -> AssetType override { return AssetType::MeshCollider; }
        auto OnUpdate() -> Signal<const MeshColliderUpdateEventData&>& { return m_onUpdate; }

    private:
        StringTable m_map;
        std::string m_assetDirectory;
        Signal<const MeshColliderUpdateEventData&> m_onUpdate;
};
} // namespace nc::asset
