#pragma once

#include <string>
#include <vector>

namespace nc::graphics
{
    struct TextureIndices
    {
        uint32_t baseColor;
        uint32_t normal;
        uint32_t roughness;
    };

    void LoadTextures(const std::vector<std::string>& paths);
    void LoadTexture(const std::string& path);
}