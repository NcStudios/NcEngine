#include "EnumExtensions.h"

#include "fmt/format.h"
#include "ncutility/NcError.h"

#include <algorithm>
#include <utility>

namespace nc::convert
{
auto CanOutputMany(asset::AssetType type) -> bool
{
    return type == asset::AssetType::Mesh || type == asset::AssetType::SkeletalAnimation || type == asset::AssetType::ShapeKeyAnimation;
}

auto ToAssetType(std::string type) -> asset::AssetType
{
    std::ranges::transform(type, type.begin(), [](char c) { return std::tolower(c); });

    if(type == "audio-clip")
        return asset::AssetType::AudioClip;
    else if(type == "convex-hull")
        return asset::AssetType::ConvexHull;
    else if(type == "cube-map")
        return asset::AssetType::CubeMap;
    else if(type == "mesh")
        return asset::AssetType::Mesh;
    else if(type == "mesh-collider")
        return asset::AssetType::MeshCollider;
    else if(type == "shapekey-animation")
        return asset::AssetType::ShapeKeyAnimation;
    else if(type == "skeletal-animation")
        return asset::AssetType::SkeletalAnimation;
    else if(type == "texture")
        return asset::AssetType::Texture;

    throw NcError("Failed to parse asset type from: " + type);
}

auto ToString(asset::AssetType type) -> std::string_view
{
    switch(type)
    {
        case asset::AssetType::AudioClip:
            return "audio-clip";
        case asset::AssetType::ConvexHull:
            return "convex-hull";
        case asset::AssetType::CubeMap:
            return "cube-map";
        case asset::AssetType::Mesh:
            return "mesh";
        case asset::AssetType::MeshCollider:
            return "mesh-collider";
        case asset::AssetType::ShapeKeyAnimation:
            return "shapekey-animation";
        case asset::AssetType::SkeletalAnimation:
            return "skeletal-animation";
        case asset::AssetType::Texture:
            return "texture";
        default:
            break;
    }

    throw NcError(
        fmt::format("Unknown AssetType: {}", std::to_underlying(type))
    );
}

auto IsCompressedTextureFormat(asset::TextureFormat format) -> bool
{
    using enum asset::TextureFormat;
    switch (format)
    {
        case RGBA8_UNORM_SRGB: [[fallthrough]];
        case RGBA8_UNORM:      return false;
        case BC1_UNORM_SRGB:   [[fallthrough]];
        case BC1_UNORM:        [[fallthrough]];
        case BC3_UNORM_SRGB:   [[fallthrough]];
        case BC3_UNORM:        return true;
        case UNKNOWN:          break;
    }

    throw NcError{
        fmt::format("Unknown TextureFormat: {}", std::to_underlying(format))
    };
}

auto TextureFormatHasAlpha(asset::TextureFormat format) -> bool
{
    using enum asset::TextureFormat;
    switch (format)
    {
        case RGBA8_UNORM_SRGB: [[fallthrough]];
        case RGBA8_UNORM:      [[fallthrough]];
        case BC3_UNORM_SRGB:   [[fallthrough]];
        case BC3_UNORM:        return true;
        case BC1_UNORM_SRGB:   [[fallthrough]];
        case BC1_UNORM:        return false;
        case UNKNOWN:          break;
    }

    throw NcError{
        fmt::format("Unknown TextureFormat: {}", std::to_underlying(format))
    };
}

auto GetMinimumDimension(asset::TextureFormat format) -> uint32_t
{
    using enum asset::TextureFormat;
    switch (format)
    {
        case RGBA8_UNORM_SRGB: [[fallthrough]];
        case RGBA8_UNORM:      return 1u;
        case BC3_UNORM_SRGB:   [[fallthrough]];
        case BC3_UNORM:        [[fallthrough]];
        case BC1_UNORM_SRGB:   [[fallthrough]];
        case BC1_UNORM:        return 4u;
        case UNKNOWN:          break;
    }

    throw NcError{
        fmt::format("Unknown TextureFormat: {}", std::to_underlying(format))
    };
}

auto ToTextureFormat(const std::string& type) -> asset::TextureFormat
{
    using enum asset::TextureFormat;
    if      (type == "UNKNOWN")          return UNKNOWN;
    else if (type == "RGBA8_UNORM_SRGB") return RGBA8_UNORM_SRGB;
    else if (type == "RGBA8_UNORM")      return RGBA8_UNORM;
    else if (type == "BC1_UNORM_SRGB")   return BC1_UNORM_SRGB;
    else if (type == "BC1_UNORM")        return BC1_UNORM;
    else if (type == "BC3_UNORM_SRGB")   return BC3_UNORM_SRGB;
    else if (type == "BC3_UNORM")        return BC3_UNORM;

    throw NcError{
        fmt::format("Unknown TextureFormat: {}", type)
    };
}

auto ToString(asset::TextureFormat format) -> std::string_view
{
    using enum asset::TextureFormat;
    switch (format)
    {
        case UNKNOWN:          return "UNKNOWN";
        case RGBA8_UNORM_SRGB: return "RGBA8_UNORM_SRGB";
        case RGBA8_UNORM:      return "RGBA8_UNORM";
        case BC1_UNORM_SRGB:   return "BC1_UNORM_SRGB";
        case BC1_UNORM:        return "BC1_UNORM";
        case BC3_UNORM_SRGB:   return "BC3_UNORM_SRGB";
        case BC3_UNORM:        return "BC3_UNORM";
    }

    throw NcError{
        fmt::format("Unknown TextureFormat: {}", std::to_underlying(format))
    };
}
} // namespace nc::convert
