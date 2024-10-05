#include "EnumExtensions.h"

#include "fmt/format.h"
#include "ncutility/NcError.h"

#include <algorithm>

namespace nc::convert
{
auto CanOutputMany(asset::AssetType type) -> bool
{
    return type == asset::AssetType::Mesh || type == asset::AssetType::SkeletalAnimation;
}

auto ToAssetType(std::string type) -> asset::AssetType
{
    std::ranges::transform(type, type.begin(), [](char c) { return std::tolower(c); });

    if(type == "audio-clip")
        return asset::AssetType::AudioClip;
    else if(type == "concave-collider")
        return asset::AssetType::ConcaveCollider;
    else if(type == "cube-map")
        return asset::AssetType::CubeMap;
    else if(type == "hull-collider")
        return asset::AssetType::HullCollider;
    else if(type == "mesh")
        return asset::AssetType::Mesh;
    else if(type == "skeletal-animation")
        return asset::AssetType::SkeletalAnimation;
    else if(type == "texture")
        return asset::AssetType::Texture;

    throw NcError("Failed to parse asset type from: " + type);
}

auto ToString(asset::AssetType type) -> std::string
{
    switch(type)
    {
        case asset::AssetType::AudioClip:
            return "audio-clip";
        case asset::AssetType::CubeMap:
            return "cube-map";
        case asset::AssetType::ConcaveCollider:
            return "concave-collider";
        case asset::AssetType::HullCollider:
            return "hull-collider";
        case asset::AssetType::Mesh:
            return "mesh";
        case asset::AssetType::SkeletalAnimation:
            return "skeletal-animation";
        case asset::AssetType::Texture:
            return "texture";
        default:
            break;
    }

    throw NcError(
        fmt::format("Unknown AssetType: {}", static_cast<int>(type))
    );
}
} // namespace nc::convert
