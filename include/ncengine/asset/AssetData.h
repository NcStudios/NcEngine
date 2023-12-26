/**
 * @file AssetData.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

#include "Assets.h"
#include "ncasset/Assets.h"

#include <span>
#include <string>
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

/** @brief Event data for mesh bones/rig load and unload operations. */
struct BoneUpdateEventData
{
    std::span<const asset::BonesData> data;
    std::vector<std::string> ids;
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
    CubeMapUpdateEventData(UpdateAction updateAction_,
                           std::vector<std::string> ids_,
                           std::span<const CubeMapWithId> data_);

    std::vector<std::string> ids;
    std::span<const CubeMapWithId> data;
    UpdateAction updateAction;
};

/** @brief Event data for mesh load and unload operations. */
struct MeshUpdateEventData
{
    std::span<const asset::MeshVertex> vertices;
    std::span<const uint32_t> indices;
};

/** @brief Event data for skeletal animation load and unload operations. */
struct SkeletalAnimationUpdateEventData
{
    std::span<const std::string> ids;
    std::span<const SkeletalAnimation> data;
    UpdateAction updateAction;
};

/** @brief A data/id pair for a loaded texture. */
struct TextureWithId
{
    asset::Texture texture;
    std::string id;
    asset_flags_type flags;
};

/** @brief Event data for texture load and unload operations. */
struct TextureUpdateEventData
{
    TextureUpdateEventData(UpdateAction updateAction_,
                           std::vector<std::string> ids_,
                           std::span<const TextureWithId> data_);

    std::vector<std::string> ids;
    std::span<const TextureWithId> data;
    UpdateAction updateAction;
};
} // namespace nc::asset
