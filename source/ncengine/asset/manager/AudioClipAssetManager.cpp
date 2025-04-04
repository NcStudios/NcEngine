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

auto AudioClipAssetManager::Load(const std::string& path) -> bool
{
    if (IsLoaded(path))
    {
        return false;
    }

    const auto fullPath = m_assetDirectory + path;
    m_audioClips.emplace(path, asset::ImportAudioClip(fullPath));
    return true;
}

auto AudioClipAssetManager::Load(std::span<const std::string> paths) -> bool
{
    bool anyLoaded = false;

    for(const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        if (Load(path))
        {
            anyLoaded = true;
        }
    }

    return anyLoaded;
}

auto AudioClipAssetManager::Unload(const std::string& path) -> bool
{
    return m_audioClips.erase(path);
}

void AudioClipAssetManager::UnloadAll()
{
    m_audioClips.clear();
}

auto AudioClipAssetManager::Acquire(const std::string& path) const -> AudioClipView
{
    NC_ASSERT(m_audioClips.contains(path), fmt::format("AudioClip is not loaded: '{}'", path));
    return Acquire(m_audioClips.hash(path));
}

auto AudioClipAssetManager::Acquire(AssetId id) const -> AudioClipView
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

auto AudioClipAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_audioClips.keys());
}
} //namespace nc::asset
