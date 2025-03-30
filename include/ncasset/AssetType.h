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

/**
 * @brief Texture formats
 * 
 * Compressed formats require input images to be at least 4x4 and have dimensions that are powers of two. This
 * is enforced via resizing in the converter. BC1 is ideal for fully opaque images, while BC3 encodes alpha via
 * interpolation.
 */
enum class TextureFormat : uint8_t
{
    RGBA8_UNORM_SRGB, ///< 32-bit unorm sRGB
    RGBA8_UNORM,      ///< 32-bit unorm
    BC1_UNORM_SRGB,   ///< 32-bit unorm sRGB compressed with BC1 (DXT1)
    BC1_UNORM,        ///< 32-bit unorm compressed with BC1 (DXT1)
    BC3_UNORM_SRGB,   ///< 32-bit unorm sRGB compressed with BC3 (DXT5)
    BC3_UNORM         ///< 32-bit unorm compressed with BC3 (DXT5)
};

} // namespace nc::asset
