/**
 * @file AssetData.h
 * @copyright Jaremie Romer and McCallister Romer 2025
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
    std::span<const BonesData> data;
    std::vector<std::string> ids;
    UpdateAction updateAction;
};

/** @brief A data/id pair for a loaded CubeMap. */
struct CubeMapWithId
{
    CubeMap cubeMap;
    size_t id;
};

/** @brief Event data for cubemap load and unload operations. */
struct CubeMapUpdateEventData
{
    CubeMapUpdateEventData(UpdateAction updateAction_,
                           std::span<const CubeMapWithId> data_);

    std::span<const CubeMapWithId> data;
    UpdateAction updateAction;
};

/** @brief Event data for mesh load and unload operations. */
struct MeshUpdateEventData
{
    std::span<const MeshVertex> vertices;
    std::span<const uint32_t> indices;
};

/** @brief A data/id pair for a loaded texture. */
template <typename T>
struct TextureWithId
{
    Texture<T> texture;
    size_t id;
};

/** @brief A data/id pair for a loaded shape key animation. */
struct ShapeKeyAnimationWithId
{
    TextureWithId<float> animation;
    float durationInSeconds;
    uint32_t numShapeKeys;
    size_t index;
};

/** @brief Event data for shape key animation load and unload operations. */
struct ShapeKeyAnimationUpdateEventData
{
    ShapeKeyAnimationUpdateEventData(UpdateAction updateAction_,
                                     std::span<const ShapeKeyAnimationWithId> data_);

    std::span<const ShapeKeyAnimationWithId> data;
    UpdateAction updateAction;
};

/** @brief Event data for skeletal animation load and unload operations. */
struct SkeletalAnimationUpdateEventData
{
    std::span<const std::string> ids;
    std::span<const SkeletalAnimation> data;
    UpdateAction updateAction;
};

/** @brief Event data for texture load and unload operations. */
struct TextureUpdateEventData
{
    TextureUpdateEventData(UpdateAction updateAction_,
                           std::span<const TextureWithId<unsigned char>> data_);

    std::span<const TextureWithId<unsigned char>> data;
    UpdateAction updateAction;
};

/** @brief Event data for ConvexHull load and unload operations. */
struct ConvexHullUpdateEventData
{
    std::span<const ConvexHull> convexHulls;
    std::span<const AssetId> ids;
    UpdateAction updateAction;
};

/** @brief Event data for MeshCollider load and unload operations. */
struct MeshColliderUpdateEventData
{
    std::span<const MeshCollider> colliders;
    std::span<const AssetId> ids;
    UpdateAction updateAction;
};
} // namespace nc::asset
