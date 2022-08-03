#pragma once

#include "graphics/Vertex.h"
#include "stb/stb_image.h"

#include <span>
#include <string>
#include <vector>

namespace nc
{
struct MeshBufferData
{
    std::span<const Vertex> vertices;
    std::span<const uint32_t> indices;
};

struct TextureData
{
    TextureData(unsigned char* pixels_, int32_t width_, int32_t height_);

    struct Deleter
    {
        void operator()(unsigned char* pixels)
        {
            stbi_image_free(pixels);
        }
    };

    unsigned char* pixels;
    int32_t width;
    int32_t height;
};

struct TextureBufferData
{
    std::vector<std::string> ids;
    std::vector<TextureData> data;
};


// #include <memory>
// #include <cstdlib>

// int main()
// {
//     int* ptr = (int*)malloc(sizeof(int));
//     auto myPtr = std::unique_ptr<int, decltype(&::free)>(ptr, free);

//     return 0;

// }
}