/**
 * @file AssetType.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include <cstdint>

namespace nc::asset
{
/** @brief Indicates type of an asset. */
enum class AssetType
{
    AudioClip,
    ConvexHull,
    CubeMap,
    Mesh,
    MeshCollider,
    Shader,
    SkeletalAnimation,
    Texture,
    Font
};

/** @brief Indicates special properties of asset. */
enum class AssetSubtype : uint8_t
{
    None,
    ColorTexture,
    NormalTexture
};
} // namespace nc::asset
