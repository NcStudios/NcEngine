#pragma once

#include "assets/AssetService.h"
#include "utility/StringMap.h"

#include "ncasset/AssetsFwd.h"

#include <unordered_map>

namespace nc
{
class ConcaveColliderAssetManager : public IAssetService<ConcaveColliderView, std::string>
{
    public:
        explicit ConcaveColliderAssetManager(const std::string& concaveColliderAssetDirectory);

        bool Load(const std::string& path, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const std::string& path, asset_flags_type flags = AssetFlags::None) override;
        void UnloadAll(asset_flags_type flags = AssetFlags::None) override;
        auto Acquire(const std::string& path, asset_flags_type flags = AssetFlags::None) const -> ConcaveColliderView override;
        bool IsLoaded(const std::string& path, asset_flags_type flags = AssetFlags::None) const override;
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto GetAssetType() const noexcept -> asset::AssetType override { return asset::AssetType::ConcaveCollider; }

    private:
        StringMap<asset::ConcaveCollider> m_concaveColliders;
        std::string m_assetDirectory;
};
} // namespace nc
