#pragma once

#include "AssetService.h"

#include <unordered_map>

namespace nc
{
    struct SoundClipFlyweight
    {
        std::vector<double> leftChannel;
        std::vector<double> rightChannel;
        size_t samplesPerChannel;
    };

    class SoundClipAssetManager : public IAssetService<SoundClipView>
    {
        protected:
            bool Load(const std::string& path) override;
            bool Load(const std::vector<std::string>& paths) override;
            bool Unload(const std::string& path) override;
            auto Acquire(const std::string& path) const -> SoundClipView override;
            bool IsLoaded(const std::string& path) const override;
        
        private:
            std::unordered_map<std::string, SoundClipFlyweight> m_soundClips;
    };
}