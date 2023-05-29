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
struct TextureWithId
{
    asset::Texture texture;
    std::string id;
    bool isNormalMap;
};

/** @brief Event data for texture load and unload operations. */
struct TextureUpdateEventData
{
    TextureUpdateEventData(UpdateAction updateAction_, std::vector<std::string> ids_, std::span<const TextureWithId> data_);

    std::vector<std::string> ids;
    std::span<const TextureWithId> data;
    UpdateAction updateAction;
};

/** @brief A data/id pair for a loaded CubeMap. */
struct CubeMapWithId
{
    asset::CubeMap cubeMap;
    std::string id;
};

/** @brief Event data for cubemap load and unload operations. */
struct CubeMapUpdateEventData
{
    CubeMapUpdateEventData(UpdateAction updateAction_, std::vector<std::string> ids_, std::span<const CubeMapWithId> data_);

    std::vector<std::string> ids;
    std::span<const CubeMapWithId> data;
    UpdateAction updateAction;
};

/** @brief A data/id pair for a loaded normal map. */
struct NormalMapWithId
{
    asset::NormalMap normalMap;
    std::string id;
};

/** @brief Event data for texture load and unload operations. */
struct NormalMapUpdateEventData
{
    NormalMapUpdateEventData(UpdateAction updateAction_, std::vector<std::string> ids_, std::span<const NormalMapWithId> data_);

    std::vector<std::string> ids;
    std::span<const NormalMapWithId> data;
    UpdateAction updateAction;
};
} // namespace nc::asset
