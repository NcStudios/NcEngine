#pragma once

#include "asset/AssetService.h"
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

        auto Load(const std::string& path,
                  AssetSubtype = AssetSubtype::None) -> bool                          override;
        auto Load(std::span<const std::string> paths,
                  AssetSubtype = AssetSubtype::None) -> bool                          override;
        auto Unload(const std::string& path)         -> bool                          override;
        void UnloadAll()                                                              override;
        auto Acquire(const std::string& path)  const -> AudioClipView                 override;
        auto Acquire(AssetId id)               const -> AudioClipView                 override;
        auto GetAllLoaded()                    const -> std::vector<std::string_view> override;
        auto IsLoaded(const std::string& path) const -> bool                          override { return m_audioClips.contains(path); }
        auto GetPath(AssetId id)               const -> std::string_view              override { return m_audioClips.key_at(id); }
        auto GetAssetType()                    const -> AssetType                     override { return AssetType::AudioClip; }

    private:
        StringMap<AudioClip> m_audioClips;
        std::string m_assetDirectory;
};
} // namespace nc::asset

