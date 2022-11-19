#pragma once

#include "graphics/meshes/Vertex.h"
#include "utility/Memory.h"

#include <array>
#include <cstdlib>
#include <memory>
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
    TextureData(unsigned char* pixels_, int width_, int height_, std::string id_);
    unique_c_ptr<unsigned char[]> pixels;
    int width;
    int height;
    std::string id;
};

struct TextureBufferData
{
    TextureBufferData(UpdateAction updateAction_, std::vector<std::string> ids_, std::span<const TextureData> data_);
    std::vector<std::string> ids;
    std::span<const TextureData> data;
    UpdateAction updateAction;
};

struct CubeMapData
{
    CubeMapData(const std::array<unsigned char*, 6>& pixels, int width_, int height_, int size_, std::string id_);
    std::array<unique_c_ptr<unsigned char[]>, 6> pixelArray;
    int width;
    int height;
    int size;
    std::string id;
};

struct CubeMapBufferData
{
    CubeMapBufferData(UpdateAction updateAction_, std::vector<std::string> ids_, std::span<const CubeMapData> data_);
    std::vector<std::string> ids;
    std::span<const CubeMapData> data;
    UpdateAction updateAction;
};
} // namespace nc