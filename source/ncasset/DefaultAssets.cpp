#include "ncasset/DefaultAssets.h"

#include <array>

namespace nc::asset
{
auto GetDefaultAudioClipPaths() -> std::span<const std::string_view>
{
    static constexpr auto paths = std::array{
        std::string_view{DefaultAudioClip}
    };

    return paths;
}

auto GetDefaultConvexHullPaths() -> std::span<const std::string_view>
{
    static constexpr auto paths = std::array{
        std::string_view{DefaultConvexHull}
    };

    return paths;
}

auto GetDefaultCubeMapPaths() -> std::span<const std::string_view>
{
    static constexpr auto paths = std::array{
        std::string_view{DefaultSkyboxCubeMap}
    };

    return paths;
}

auto GetDefaultMeshPaths() -> std::span<const std::string_view>
{
    static constexpr auto paths = std::array{
        std::string_view{PlaneMesh},
        std::string_view{CubeMesh},
        std::string_view{SphereMesh},
        std::string_view{CapsuleMesh},
        std::string_view{WheelMesh},
        std::string_view{SkyboxMesh}
    };

    return paths;
}

auto GetDefaultMeshColliderPaths() -> std::span<const std::string_view>
{
    static constexpr auto paths = std::array{
        std::string_view{DefaultMeshCollider}
    };

    return paths;
}

auto GetDefaultSkeletalAnimationPaths() -> std::span<const std::string_view>
{
    static constexpr auto paths = std::array{
        std::string_view{DefaultSkeletalAnimation}
    };

    return paths;
}

auto GetDefaultTexturePaths() -> std::span<const std::string_view>
{
    static constexpr auto paths = std::array{
        std::string_view{DefaultBaseColor},
        std::string_view{DefaultNormal},
        std::string_view{DefaultParticle}
    };

    return paths;
}

auto GetDefaultTextureSubtypes() -> std::span<const AssetSubtype>
{
    static constexpr auto subtypes = std::array{
        AssetSubtype::ColorTexture,
        AssetSubtype::ColorTexture,
        AssetSubtype::NormalTexture,
        AssetSubtype::ColorTexture
    };

    return subtypes;
}
} // namespace nc::asset
