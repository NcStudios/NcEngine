#pragma once

#include "ncasset/AssetsFwd.h"

#include <filesystem>

namespace nc::convert
{
class AudioConverter
{
    public:
        auto ImportAudioClip(const std::filesystem::path& path) -> asset::AudioClip;
};
} // namespace nc::convert
