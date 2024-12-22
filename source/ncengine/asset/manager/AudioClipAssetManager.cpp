#include "AudioClipAssetManager.h"
#include "AssetUtilities.h"

#include "ncasset/Import.h"

namespace nc::asset
{
AudioClipAssetManager::AudioClipAssetManager(const std::string& assetDirectory)
    : m_audioClips{},
      m_assetDirectory{assetDirectory}
{
}

bool AudioClipAssetManager::Load(const std::string& path, bool isExternal, asset_flags_type)
{
    if (IsLoaded(path))
    {
        return false;
    }

    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    m_audioClips.emplace(path, asset::ImportAudioClip(fullPath));
    return true;
}

bool AudioClipAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type)
{
    bool anyLoaded = false;

    for(const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        if (Load(path, isExternal))
        {
            anyLoaded = true;
        }
    }

    return anyLoaded;
}

bool AudioClipAssetManager::Unload(const std::string& path, asset_flags_type)
{
    return m_audioClips.erase(path);
}

void AudioClipAssetManager::UnloadAll(asset_flags_type)
{
    m_audioClips.clear();
}

auto AudioClipAssetManager::Acquire(const std::string& path, asset_flags_type) const -> AudioClipView
{
    NC_ASSERT(m_audioClips.contains(path), fmt::format("AudioClip is not loaded: '{}'", path));
    return Acquire(m_audioClips.hash(path));
}

auto AudioClipAssetManager::Acquire(AssetId id, asset_flags_type) const -> AudioClipView
{
    const auto index = m_audioClips.index(id);
    NC_ASSERT(index != m_audioClips.NullIndex, fmt::format("AudioClip is not loaded: '{}'", id));
    const auto& clip = m_audioClips.at(index);
    return AudioClipView{
        .id = id,
        .leftChannel = std::span<const double>{clip.leftChannel},
        .rightChannel = std::span<const double>{clip.rightChannel},
        .samplesPerChannel = clip.samplesPerChannel
    };
}

bool AudioClipAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
{
    return m_audioClips.contains(path);
}

auto AudioClipAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_audioClips.keys());
}
} //namespace nc::asset
