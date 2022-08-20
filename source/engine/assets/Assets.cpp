#include "asset/Assets.h"
#include "AssetService.h"

namespace nc
{
    bool LoadAudioClipAsset(const std::string& path, bool isExternal)
    {
        return AssetService<AudioClipView>::Get()->Load(path, isExternal);
    }

    bool LoadAudioClipAssets(std::span<const std::string> paths, bool isExternal)
    {
        return AssetService<AudioClipView>::Get()->Load(paths, isExternal);
    }

    bool UnloadAudioClipAsset(const std::string& path)
    {
        return AssetService<AudioClipView>::Get()->Unload(path);
    }

    void UnloadAllAudioClipAssets()
    {
        AssetService<AudioClipView>::Get()->UnloadAll();
    }

    bool LoadConvexHullAsset(const std::string& path, bool isExternal)
    {
        return AssetService<ConvexHullView>::Get()->Load(path, isExternal);
    }

    bool LoadConvexHullAssets(std::span<const std::string> paths, bool isExternal)
    {
        return AssetService<ConvexHullView>::Get()->Load(paths, isExternal);
    }

    bool UnloadConvexHullAsset(const std::string& path)
    {
        return AssetService<ConvexHullView>::Get()->Unload(path);
    }

    void UnloadAllConvexHullAssets()
    {
        AssetService<ConvexHullView>::Get()->UnloadAll();
    }

    bool LoadConcaveColliderAsset(const std::string& path, bool isExternal)
    {
        return AssetService<ConcaveColliderView>::Get()->Load(path, isExternal);
    }

    bool LoadConcaveColliderAssets(std::span<const std::string> paths, bool isExternal)
    {
        return AssetService<ConcaveColliderView>::Get()->Load(paths, isExternal);
    }

    bool UnloadConcaveColliderAsset(const std::string& path)
    {
        return AssetService<ConcaveColliderView>::Get()->Unload(path);
    }

    void UnloadAllConcaveColliderAssets()
    {
        AssetService<ConcaveColliderView>::Get()->UnloadAll();
    }

    bool LoadMeshAsset(const std::string& path, bool isExternal)
    {
        return AssetService<MeshView>::Get()->Load(path, isExternal);
    }

    bool LoadMeshAssets(std::span<const std::string> paths, bool isExternal)
    {
        return AssetService<MeshView>::Get()->Load(paths, isExternal);
    }

    bool UnloadMeshAsset(const std::string& path)
    {
        return AssetService<MeshView>::Get()->Unload(path);
    }

    void UnloadAllMeshAssets()
    {
        AssetService<MeshView>::Get()->UnloadAll();
    }

    bool LoadTextureAsset(const std::string& path, bool isExternal)
    {
        return AssetService<TextureView>::Get()->Load(path, isExternal);
    }

    bool LoadTextureAssets(std::span<const std::string> paths, bool isExternal)
    {
        return AssetService<TextureView>::Get()->Load(paths, isExternal);
    }

    bool UnloadTextureAsset(const std::string& path)
    {
        return AssetService<TextureView>::Get()->Unload(path);
    }

    void UnloadAllTextureAssets()
    {
        return AssetService<TextureView>::Get()->UnloadAll();
    }

    bool LoadCubeMapAsset(const std::string& path, bool isExternal)
    {
        return AssetService<CubeMapView>::Get()->Load(path, isExternal);
    }

    bool LoadCubeMapAssets(std::span<const std::string> paths, bool isExternal)
    {
        return AssetService<CubeMapView>::Get()->Load(paths, isExternal);
    }

    bool UnloadCubeMapAsset(const std::string& path)
    {
        return AssetService<CubeMapView>::Get()->Unload(path);
    }

    void UnloadAllCubeMapAssets()
    {
        return AssetService<CubeMapView>::Get()->UnloadAll();
    }

} // namespace nc