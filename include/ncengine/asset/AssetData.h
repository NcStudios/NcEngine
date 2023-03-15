#pragma once

#include "ncasset/Assets.h"

#include <cstdlib>
#include <span>
#include <string>
#include <variant>
#include <vector>

namespace nc::asset
{
/** @brief Indicates the type of an asset update event. */
enum class UpdateAction : uint8_t
{
    Load,
    Unload,
    UnloadAll
};

/** @brief Event data for mesh load and unload operations. */
struct MeshUpdateEventData
{
    std::span<const asset::MeshVertex> vertices;
    std::span<const uint32_t> indices;
};

/** @brief A data/id pair for a loaded texture. */
struct TaggedTexture
{
    asset::Texture texture;
    std::string id;
};

/** @brief Event data for texture load and unlaod operations. */
struct TextureUpdateEventData
{
    TextureUpdateEventData(UpdateAction updateAction_, std::vector<std::string> ids_, std::span<const TaggedTexture> data_);

    std::vector<std::string> ids;
    std::span<const TaggedTexture> data;
    UpdateAction updateAction;
};

/** @brief A data/id pair for a loaded CubeMap. */
struct TaggedCubeMap
{
    asset::CubeMap cubeMap;
    std::string id;
};

/** @brief Event data for cubemap load and unload operations. */
struct CubeMapUpdateEventData
{
    CubeMapUpdateEventData(UpdateAction updateAction_, std::vector<std::string> ids_, std::span<const TaggedCubeMap> data_);

    std::vector<std::string> ids;
    std::span<const TaggedCubeMap> data;
    UpdateAction updateAction;
};
} // namespace nc::asset
