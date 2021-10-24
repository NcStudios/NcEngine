#pragma once

#include "physics/Geometry.h"

#include <concepts>
#include <span>
#include <string>

namespace nc
{
    /** Loading assets creates an internal mapping between the asset path and data.
     *  Duplicate loads are ignored. Objects that use assets do not load them on
     *  demand, nor do they own the asset data. Assets must be loaded before dependent
     *  objects are created and should be unloaded only when they are no longer in use. */

    /** Supported file types: .nca */
    bool LoadConcaveColliderAsset(const std::string& path);
    bool LoadConcaveColliderAssets(std::span<const std::string> paths);
    bool UnloadConcaveColliderAsset(const std::string& path);
    void UnloadAllConcaveColliderAssets();

    /** Supported file types: .nca */
    bool LoadConvexHullAsset(const std::string& path);
    bool LoadConvexHullAssets(std::span<const std::string> paths);
    bool UnloadConvexHullAsset(const std::string& path);
    void UnloadAllConvexHullAssets();

    /** Supported file types: .nca 
     *  @note Unloading meshes invalidates all MeshViews. It is intended
     *  to be done on scene change. */
    bool LoadMeshAsset(const std::string& path);
    bool LoadMeshAssets(std::span<const std::string> paths);
    bool UnloadMeshAsset(const std::string& path);
    void UnloadAllMeshAssets();

    /** Supported file types: .wav */
    bool LoadSoundClipAsset(const std::string& path);
    bool LoadSoundClipAssets(std::span<const std::string> paths);
    bool UnloadSoundClipAsset(const std::string& path);
    void UnloadAllSoundClipAssets();

    /** Supported file types: .png 
     *  @note Unloading textures invalidates all TextureViews. It is intended
     *  to be done on scene change. */
    bool LoadTextureAsset(const std::string& path);
    bool LoadTextureAssets(std::span<const std::string> paths);
    bool UnloadTextureAsset(const std::string& path);
    void UnloadAllTextureAssets();

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

    struct SoundClipView
    {
        std::span<const double> leftChannel;
        std::span<const double> rightChannel;
        size_t samplesPerChannel;
    };

    struct TextureView
    {
        uint32_t index;
    };

    /** Restrict instantiations to supported asset types to minimize
     *  errors with the service locator. */
    template<class T>
    concept AssetType = std::same_as<T, SoundClipView> ||
                        std::same_as<T, ConvexHullView> ||
                        std::same_as<T, ConcaveColliderView> ||
                        std::same_as<T, MeshView> ||
                        std::same_as<T, TextureView>;
}