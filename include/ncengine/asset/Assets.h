#pragma once

#include "ncengine/utility/EnumUtilities.h"

#include "ncmath/Geometry.h"

#include <concepts>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace nc
{
    /** Asset flags are used to pass parameters to the asset loading/unloading functions. */
    using asset_flags_type = uint64_t;
    struct AssetFlags
    {
        static constexpr asset_flags_type None                 = 0b00000000;
        static constexpr asset_flags_type TextureTypeImage     = 0b00000001;
        static constexpr asset_flags_type TextureTypeNormalMap = 0b00000010;
    };

    /** Assets must be loaded before dependent objects are created and should be unloaded only
     *  when they are no longer in use.
     * 
     *  Paths should be relative to directories specified in the config. Passing true for isExternal
     *  allows paths to be absolute or relative to the executable. Duplicate loads are ignored. */

    /** Supported file types: .nca */
    bool LoadConcaveColliderAsset(const std::string& path, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
    bool LoadConcaveColliderAssets(std::span<const std::string> paths, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
    bool UnloadConcaveColliderAsset(const std::string& path, asset_flags_type flags = AssetFlags::None);
    void UnloadAllConcaveColliderAssets(asset_flags_type flags = AssetFlags::None);

    /** Supported file types: .nca */
    bool LoadConvexHullAsset(const std::string& path, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
    bool LoadConvexHullAssets(std::span<const std::string> paths, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
    bool UnloadConvexHullAsset(const std::string& path, asset_flags_type flags = AssetFlags::None);
    void UnloadAllConvexHullAssets(asset_flags_type flags = AssetFlags::None);

    /** Supported file types: .nca 
     *  @note Unloading meshes invalidates all MeshViews. It is intended
     *  to be done on scene change. */
    bool LoadMeshAsset(const std::string& path, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
    bool LoadMeshAssets(std::span<const std::string> paths, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
    bool UnloadMeshAsset(const std::string& path, asset_flags_type flags = AssetFlags::None);
    void UnloadAllMeshAssets(asset_flags_type flags = AssetFlags::None);

    /** Supported file types: .wav */
    bool LoadAudioClipAsset(const std::string& path, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
    bool LoadAudioClipAssets(std::span<const std::string> paths, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
    bool UnloadAudioClipAsset(const std::string& path, asset_flags_type flags = AssetFlags::None);
    void UnloadAllAudioClipAssets(asset_flags_type flags = AssetFlags::None);

    /** Supported file types: .png 
     *  @note Unloading textures invalidates all TextureViews. It is intended
     *  to be done on scene change. */
    bool LoadTextureAsset(const std::string& path, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
    bool LoadTextureAssets(std::span<const std::string> paths, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
    bool UnloadTextureAsset(const std::string& path, asset_flags_type flags = AssetFlags::None);
    void UnloadAllTextureAssets(asset_flags_type flags = AssetFlags::None);

    /** Supported file types: .png 
    *  @note Unloading textures invalidates all CubeMapViews. It is intended
    *  to be done on scene change. */
    bool LoadCubeMapAsset(const std::string& path, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
    bool LoadCubeMapAssets(std::span<const std::string> paths, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
    bool UnloadCubeMapAsset(const std::string& paths, asset_flags_type flags = AssetFlags::None);
    void UnloadAllCubeMapAssets(asset_flags_type flags = AssetFlags::None);

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

    struct MeshView
    {
        uint32_t firstVertex;
        uint32_t vertexCount;
        uint32_t firstIndex;
        uint32_t indexCount;
        float maxExtent;
    };

    struct AudioClipView
    {
        std::span<const double> leftChannel;
        std::span<const double> rightChannel;
        size_t samplesPerChannel;
    };

    struct TextureView
    {
        uint32_t index;
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

    /** Restrict instantiations to supported asset types to minimize
     *  errors with the service locator. */
    template<class T>
    concept AssetView = std::same_as<T, AudioClipView> ||
                        std::same_as<T, ConvexHullView> ||
                        std::same_as<T, ConcaveColliderView> ||
                        std::same_as<T, MeshView> ||
                        std::same_as<T, TextureView> ||
                        std::same_as<T, CubeMapView> ||
                        std::same_as<T, ShaderView>;
}