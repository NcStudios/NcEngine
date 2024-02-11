#pragma once

#include "ncengine/utility/EnumUtilities.h"

#include "ncmath/Geometry.h"

#include <concepts>
#include <span>
#include <string>

struct ImFont;

namespace nc
{
struct AudioClipView
{
    std::span<const double> leftChannel;
    std::span<const double> rightChannel;
    size_t samplesPerChannel;
};

struct ConcaveColliderView
{
    std::span<const Triangle> triangles;
    float maxExtent;
};

struct ConvexHullView
{
    std::span<const Vector3> vertices;
    Vector3 extents;
    float maxExtent;
};

enum class CubeMapUsage
{
    Skybox,
    ShadowMap
};

struct CubeMapView
{
    CubeMapUsage usage;
    uint32_t index;
};

struct MeshView
{
    uint32_t firstVertex;
    uint32_t vertexCount;
    uint32_t firstIndex;
    uint32_t indexCount;
    float maxExtent;
};

struct TextureView
{
    uint32_t index;
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
    uint32_t setIndex;
    uint32_t slotIndex;
    DescriptorType descriptorType;
    ShaderStages shaderStages;
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
    uint32_t index;
};

struct Font
{
    std::string path;
    float size = 12.0f;
};

struct FontView
{
    ImFont* font;
    float size;
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
} // namespace nc
