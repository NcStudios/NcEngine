#pragma once

#include "asset/AssetService.h"
#include "utility/StringMap.h"
#include "ncengine/utility/Signal.h"

#include "ncasset/AssetsFwd.h"

namespace nc::asset
{
struct SkeletalAnimationUpdateEventData;

class SkeletalAnimationAssetManager : public IAssetService<SkeletalAnimationView, std::string>
{
    public:
        explicit SkeletalAnimationAssetManager(const std::string& skeletalAnimationAssetDirectory, uint32_t maxSkeletalAnimations);

        bool Load(const std::string& path, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const std::string& path, asset_flags_type flags = AssetFlags::None) override;
        void UnloadAll(asset_flags_type flags = AssetFlags::None) override;
        auto Acquire(const std::string& path, asset_flags_type flags = AssetFlags::None) const -> SkeletalAnimationView override;
        bool IsLoaded(const std::string& path, asset_flags_type flags = AssetFlags::None) const override;
        auto GetPath(size_t id) const -> std::string_view override { return m_table.at(m_table.index(id)); }
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto GetAssetType() const noexcept -> asset::AssetType override { return asset::AssetType::SkeletalAnimation; }
        auto OnUpdate() -> Signal<const asset::SkeletalAnimationUpdateEventData&>&;

    private:
        StringTable m_table;
        std::string m_assetDirectory;
        uint32_t m_maxSkeletalAnimationCount;
        Signal<const SkeletalAnimationUpdateEventData&> m_onUpdate;
};
} // namespace nc::asset
