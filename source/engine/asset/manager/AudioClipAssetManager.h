#pragma once

#include "assets/AssetService.h"
#include "utility/StringMap.h"

#include "ncasset/AssetsFwd.h"

#include <string>
#include <unordered_map>

namespace nc::asset
{
class AudioClipAssetManager : public IAssetService<AudioClipView, std::string>
{
    public:
        explicit AudioClipAssetManager(const std::string& assetDirectory);

        bool Load(const std::string& path, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const std::string& path, asset_flags_type flags = AssetFlags::None) override;
        void UnloadAll(asset_flags_type flags = AssetFlags::None) override;
        auto Acquire(const std::string& path, asset_flags_type flags = AssetFlags::None) const -> AudioClipView override;
        bool IsLoaded(const std::string& path, asset_flags_type flags = AssetFlags::None) const override;
        auto GetPath(size_t id) const -> std::string_view override { return m_audioClips.key_at(id); }
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto GetAssetType() const noexcept -> AssetType override { return AssetType::AudioClip; }

    private:
        StringMap<AudioClip> m_audioClips;
        std::string m_assetDirectory;
};
} // namespace nc::asset

