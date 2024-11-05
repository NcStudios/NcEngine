/**
 * @file AssetViews.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/utility/EnumUtilities.h"

#include "ncmath/Geometry.h"

#include <concepts>
#include <limits>
#include <span>
#include <string>

struct ImFont;

namespace nc::asset
{
constexpr auto NullAssetId = std::numeric_limits<size_t>::max();
constexpr auto NullAssetIndex = std::numeric_limits<uint32_t>::max();

struct AudioClipView
{
    size_t id = NullAssetId;
    std::span<const double> leftChannel;
    std::span<const double> rightChannel;
    size_t samplesPerChannel = 0ull;
};

struct ConcaveColliderView
{
    size_t id = NullAssetId;
    std::span<const Triangle> triangles;
    float maxExtent = 0.0f;
};

struct ConvexHullView
{
    size_t id = NullAssetId;
    std::span<const Vector3> vertices;
    Vector3 extents;
    float maxExtent = 0.0f;
};

enum class CubeMapUsage
{
    Skybox,
    ShadowMap
};

struct CubeMapView
{
    size_t id = NullAssetId;
    CubeMapUsage usage = CubeMapUsage::Skybox;
    uint32_t index = NullAssetIndex;
};

struct MeshView
{
    size_t id = NullAssetId;
    uint32_t firstVertex = NullAssetIndex;
    uint32_t vertexCount = NullAssetIndex;
    uint32_t firstIndex = NullAssetIndex;
    uint32_t indexCount = NullAssetIndex;
    float maxExtent = 0.0f;
};

struct TextureView
{
    size_t id = NullAssetId;
    uint32_t index = NullAssetIndex;
};

enum class DescriptorType : uint8_t
{
    None,
    UniformBuffer,
    StorageBuffer,
    CombinedImageSampler
};

enum class ShaderStages : uint8_t
{
    None     = 0,
    Vertex   = 1,
    Fragment = 2
}; DEFINE_BITWISE_OPERATORS(ShaderStages)

struct DescriptorManifest
{
    uint32_t setIndex = NullAssetIndex;
    uint32_t slotIndex = NullAssetIndex;
    DescriptorType descriptorType = DescriptorType::None;
    ShaderStages shaderStages = ShaderStages::None;
};

struct ShaderView
{
    std::string uid;
    std::span<const uint32_t> vertexByteCode;
    std::span<const uint32_t> fragmentByteCode;
    std::span<const DescriptorManifest> descriptors;
};

struct SkeletalAnimationView
{
    size_t id = NullAssetId;
    uint32_t index = NullAssetIndex;
};

struct FontInfo
{
    std::string path;
    float size = 12.0f;
};

struct FontView
{
    ImFont* font = nullptr;
    float size = 10.0f;
};

/** Restrict instantiations to supported asset types to minimize
 *  errors with the service locator. */
template<class T>
concept AssetView = std::same_as<T, AudioClipView>         ||
                    std::same_as<T, ConvexHullView>        ||
                    std::same_as<T, ConcaveColliderView>   ||
                    std::same_as<T, MeshView>              ||
                    std::same_as<T, TextureView>           ||
                    std::same_as<T, CubeMapView>           ||
                    std::same_as<T, ShaderView>            ||
                    std::same_as<T, SkeletalAnimationView> ||
                    std::same_as<T, FontView>;
} // namespace nc::asset
