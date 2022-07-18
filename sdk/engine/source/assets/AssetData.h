#pragma once

#include "graphics/Vertex.h"

#include <span>
#include <string>
#include <vector>

namespace nc
{
enum class UpdateAction : uint8_t
{
    Load,
    Unload,
    UnloadAll
};

struct MeshBufferData
{
    std::span<const Vertex> vertices;
    std::span<const uint32_t> indices;
};

struct TextureData
{
    unsigned char* pixels;
    int32_t width;
    int32_t height;
};

struct TextureBufferData
{
    TextureBufferData(UpdateAction updateAction_, std::vector<std::string> ids_, std::vector<TextureData> data_);

    std::vector<std::string> ids;
    std::vector<TextureData> data;
    UpdateAction updateAction;
};
}