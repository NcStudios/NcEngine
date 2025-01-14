/**
 * @file AssetType.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

namespace nc::asset
{
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
} // namespace nc::asset
