#pragma once

#include "physics/Geometry.h"

#include <concepts>
#include <string>
#include <vector>

namespace nc
{
    /** Loading assets creates an internal mapping between the asset path and data.
     *  Duplicate loads are ignored. Objects that use assets do not load them on
     *  demand, nor do they own the asset data. Assets must be loaded before dependent
     *  objects are created and should be unloaded only when they are no longer in use. */

    /** Supported file types: .nca */
    void LoadConcaveColliderAsset(const std::string& path);
    
    /** Supported file types: .nca */
    void LoadConvexHullAsset(const std::string& path);

    /** Supported file types: .nca */
    void LoadMesh(const std::string& path);
    void LoadMeshes(const std::vector<std::string>& paths);

    /** Supported file types: .wav */
    void LoadSoundClipAsset(const std::string& path);

    /** Supported file types: .png */
    void LoadTextures(const std::vector<std::string>& paths);
    void LoadTexture(const std::string& path);

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
        uint32_t firstIndex;
        uint32_t indicesCount;
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