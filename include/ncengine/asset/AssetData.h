#pragma once

#include "ncasset/Assets.h"

#include <cstdlib>
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
    std::span<const asset::MeshVertex> vertices;
    std::span<const uint32_t> indices;
};

struct TextureData
{
    asset::Texture texture;
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
    asset::CubeMap cubeMap;
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