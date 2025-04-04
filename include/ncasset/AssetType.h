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

/**
 * @brief Texture formats
 * 
 * Formats:
 * - RGBA8: Uncompressed signed-normalized-integer format with 8 bit channels.
 * - BC1:   RGB data compressing 4x4 blocks in 8 bytes. Ideal for typical color images with full opacity.
 * - BC3:   RGBA data compressing 4x4 blocks into 16 bytes. Ideal for color images with alpha.
 * 
 * BCn formats require input image dimension to be powers of two and be at least 4x4. This is enforced at conversion
 * time by padding the image.
 */
enum class TextureFormat : uint8_t
{
    UNKNOWN,          ///< uninitialized value
    RGBA8_UNORM_SRGB, ///< 32-bit unorm sRGB
    RGBA8_UNORM,      ///< 32-bit unorm
    BC1_UNORM_SRGB,   ///< 32-bit unorm sRGB compressed with BC1 (DXT1)
    BC1_UNORM,        ///< 32-bit unorm compressed with BC1 (DXT1)
    BC3_UNORM_SRGB,   ///< 32-bit unorm sRGB compressed with BC3 (DXT5)
    BC3_UNORM         ///< 32-bit unorm compressed with BC3 (DXT5)
};
} // namespace nc::asset
