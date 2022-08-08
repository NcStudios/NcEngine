#pragma once

#include "graphics/Vertex.h"
#include "stb/stb_image.h"

#include <memory>
#include <span>
#include <string>
#include <vector>
#include <cstdlib>

namespace nc
{
struct MeshBufferData
{
    std::span<const Vertex> vertices;
    std::span<const uint32_t> indices;
};

struct TextureData
{
    TextureData(unsigned char* pixels_, int32_t width_, int32_t height_, std::string id_);

    std::unique_ptr<unsigned char[], decltype(&::free)> pixels;
    int32_t width;
    int32_t height;
    std::string id;
};

struct TextureBufferData
{
    std::span<const TextureData> data;
};
}