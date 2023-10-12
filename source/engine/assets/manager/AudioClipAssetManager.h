#pragma once

#include "assets/AssetService.h"

#include "ncasset/AssetsFwd.h"

#include <string>
#include <unordered_map>

namespace nc
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
        auto GetAllLoaded() const -> std::vector<std::string_view> override;

    private:
        std::unordered_map<std::string, asset::AudioClip> m_audioClips;
        std::string m_assetDirectory;
};
} // namespace nc

