#include "AudioClipAssetManager.h"
#include "AssetUtilities.h"

#include "ncasset/Import.h"

namespace nc
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
    const auto hash = m_audioClips.hash(path);
    const auto index = m_audioClips.index(hash);
    NC_ASSERT(index != m_audioClips.NullIndex, fmt::format("Asset is not loaded: '{}'", path));
    const auto& clip = m_audioClips.at(index);
    return AudioClipView
    {
        .id = hash,
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
}
