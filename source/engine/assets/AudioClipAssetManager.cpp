#include "AudioClipAssetManager.h"
#include "audio/audio_file/AudioFile.h"

namespace nc
{
    AudioClipAssetManager::AudioClipAssetManager(const std::string& assetDirectory)
        : m_audioClips{},
          m_assetDirectory{assetDirectory}
    {
    }

    bool AudioClipAssetManager::Load(const std::string& path, bool isExternal)
    {
        if(IsLoaded(path))
            return false;

        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        AudioFile<double> asset;
        if(!asset.load(fullPath))
            throw NcError("Failure opening file: " + fullPath);

        size_t samplesPerChannel = asset.samples.at(0).size();

        AudioClipFlyweight data
        {
            .leftChannel = std::move(asset.samples.at(0)),
            .rightChannel = std::move(asset.samples.at(1)),
            .samplesPerChannel = samplesPerChannel
        };

        m_audioClips.emplace(path, std::move(data));
        return true;
    }

    bool AudioClipAssetManager::Load(std::span<const std::string> paths, bool isExternal)
    {
        bool anyLoaded = false;

        for(const auto& path : paths)
        {
            if(IsLoaded(path))
                continue;

            if(Load(path, isExternal))
                anyLoaded = true;
        }

        return anyLoaded;
    }

    bool AudioClipAssetManager::Unload(const std::string& path)
    {
        return static_cast<bool>(m_audioClips.erase(path));
    }

    void AudioClipAssetManager::UnloadAll()
    {
        m_audioClips.clear();
    }

    auto AudioClipAssetManager::Acquire(const std::string& path) const -> AudioClipView
    {
        const auto it = m_audioClips.find(path);
        if(it == m_audioClips.end())
            throw NcError("Asset is not loaded: " + path);
        
        return AudioClipView
        {
            .leftChannel = std::span<const double>{it->second.leftChannel},
            .rightChannel = std::span<const double>{it->second.rightChannel},
            .samplesPerChannel = it->second.samplesPerChannel
        };
    }

    bool AudioClipAssetManager::IsLoaded(const std::string& path) const
    {
        return m_audioClips.contains(path);
    }
}