#pragma once

#include "ncasset/AssetsFwd.h"

#include <filesystem>

namespace nc::convert
{
class TextureConverter
{
    public:
        auto ImportCubeMap(const std::filesystem::path& path) -> asset::CubeMap;
        auto ImportTexture(const std::filesystem::path& path) -> asset::Texture;
};
} // namespace nc::convert
