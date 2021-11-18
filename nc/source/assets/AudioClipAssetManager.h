#pragma once

#include "AssetService.h"

#include <unordered_map>
#include <vector>

namespace nc
{
    struct AudioClipFlyweight
    {
        std::vector<double> leftChannel;
        std::vector<double> rightChannel;
        size_t samplesPerChannel;
    };

    class AudioClipAssetManager : public IAssetService<AudioClipView>
    {
        public:
            AudioClipAssetManager(const std::string& assetDirectory);

            bool Load(const std::string& path, bool isExternal) override;
            bool Load(std::span<const std::string> paths, bool isExternal) override;
            bool Unload(const std::string& path) override;
            void UnloadAll() override;
            auto Acquire(const std::string& path) const -> AudioClipView override;
            bool IsLoaded(const std::string& path) const override;
        
        private:
            std::unordered_map<std::string, AudioClipFlyweight> m_audioClips;
            std::string m_assetDirectory;
    };
}