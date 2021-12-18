#pragma once

#include "physics/Geometry.h"

#include <concepts>
#include <span>
#include <string>
#include <string_view>

namespace nc
{
    /** Assets must be loaded before dependent objects are created and should be unloaded only
     *  when they are no longer in use.
     * 
     *  Paths should be relative to directories specified in the config. Passing true for isExternal
     *  allows paths to be absolute or relative to the executable. Duplicate loads are ignored. */

    /** Supported file types: .nca */
    bool LoadConcaveColliderAsset(const std::string& path, bool isExternal = false);
    bool LoadConcaveColliderAssets(std::span<const std::string> paths, bool isExternal = false);
    bool UnloadConcaveColliderAsset(const std::string& path);
    void UnloadAllConcaveColliderAssets();

    /** Supported file types: .nca */
    bool LoadConvexHullAsset(const std::string& path, bool isExternal = false);
    bool LoadConvexHullAssets(std::span<const std::string> paths, bool isExternal = false);
    bool UnloadConvexHullAsset(const std::string& path);
    void UnloadAllConvexHullAssets();

    /** Supported file types: .nca 
     *  @note Unloading meshes invalidates all MeshViews. It is intended
     *  to be done on scene change. */
    bool LoadMeshAsset(const std::string& path, bool isExternal = false);
    bool LoadMeshAssets(std::span<const std::string> paths, bool isExternal = false);
    bool UnloadMeshAsset(const std::string& path);
    void UnloadAllMeshAssets();

    /** Supported file types: .wav */
    bool LoadAudioClipAsset(const std::string& path, bool isExternal = false);
    bool LoadAudioClipAssets(std::span<const std::string> paths, bool isExternal = false);
    bool UnloadAudioClipAsset(const std::string& path);
    void UnloadAllAudioClipAssets();

    /** Supported file types: .png 
     *  @note Unloading textures invalidates all TextureViews. It is intended
     *  to be done on scene change. */
    bool LoadTextureAsset(const std::string& path, bool isExternal = false);
    bool LoadTextureAssets(std::span<const std::string> paths, bool isExternal = false);
    bool UnloadTextureAsset(const std::string& path);
    void UnloadAllTextureAssets();

    /** Supported file types: .png 
    *  @note Unloading textures invalidates all CubeMapViews. It is intended
    *  to be done on scene change. */
    struct CubeMapFaces;
    bool LoadCubeMapAsset(const CubeMapFaces& paths, bool isExternal = false);
    bool LoadCubeMapAssets(std::span<const CubeMapFaces> paths, bool isExternal = false);
    bool UnloadCubeMapAsset(const CubeMapFaces& paths);
    void UnloadAllCubeMapAssets();

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

    struct CubeMapFaces
    {
        CubeMapUsage usage;
        std::string uid;
        std::string frontPath;
        std::string backPath;
        std::string upPath;
        std::string downPath;
        std::string rightPath;
        std::string leftPath;
    };

    /** Restrict instantiations to supported asset types to minimize
     *  errors with the service locator. */
    template<class T>
    concept AssetType = std::same_as<T, AudioClipView> ||
                        std::same_as<T, ConvexHullView> ||
                        std::same_as<T, ConcaveColliderView> ||
                        std::same_as<T, MeshView> ||
                        std::same_as<T, TextureView> ||
                        std::same_as<T, CubeMapView>;

}