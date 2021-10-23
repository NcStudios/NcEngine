#include "SoundClipAssetManager.h"
#include "audio/audio_file/AudioFile.h"

namespace nc
{
    bool SoundClipAssetManager::Load(const std::string& path)
    {
        if(IsLoaded(path))
            return false;
        
        AudioFile<double> asset;
        if(!asset.load(path))
            throw NcError("Failure opening file: " + path);

        size_t samplesPerChannel = asset.samples.at(0).size();

        SoundClipFlyweight data
        {
            .leftChannel = std::move(asset.samples.at(0)),
            .rightChannel = std::move(asset.samples.at(1)),
            .samplesPerChannel = samplesPerChannel
        };

        m_soundClips.emplace(path, std::move(data));
        return true;
    }

    bool SoundClipAssetManager::Load(const std::vector<std::string>& paths)
    {
        /** @todo */
        (void)paths;
        return false;
    }
    
    bool SoundClipAssetManager::Unload(const std::string& path)
    {
        /** @todo */
        (void)path;
        return true;
    }

    auto SoundClipAssetManager::Acquire(const std::string& path) const -> SoundClipView
    {
        const auto it = m_soundClips.find(path);
        if(it == m_soundClips.end())
            throw NcError("Asset is not loaded: " + path);
        
        return SoundClipView
        {
            .leftChannel = std::span<const double>{it->second.leftChannel},
            .rightChannel = std::span<const double>{it->second.rightChannel},
            .samplesPerChannel = it->second.samplesPerChannel
        };
    }
    
    bool SoundClipAssetManager::IsLoaded(const std::string& path) const
    {
        return m_soundClips.contains(path);
    }
}