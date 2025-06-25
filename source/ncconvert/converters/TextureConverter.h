#pragma once

#include "ncasset/AssetsFwd.h"

#include <filesystem>

namespace nc::convert
{
class TextureConverter
{
    public:
        auto ImportCubeMap(const std::filesystem::path& path,
                           asset::TextureFormat format) -> asset::CubeMap;

        auto ImportTexture(const std::filesystem::path& path,
                           asset::TextureFormat format,
                           bool generateMips) -> asset::Texture<unsigned char>;
};
} // namespace nc::convert
