#include "Assets.h"
#include "AssetService.h"

namespace nc
{
    bool LoadSoundClipAsset(const std::string& path)
    {
        return AssetService<SoundClipView>::Get()->Load(path);
    }

    bool LoadSoundClipAssets(std::span<const std::string> paths)
    {
        return AssetService<SoundClipView>::Get()->Load(paths);
    }

    bool UnloadSoundClipAsset(const std::string& path)
    {
        return AssetService<SoundClipView>::Get()->Unload(path);
    }

    void UnloadAllSoundClipAssets()
    {
        AssetService<SoundClipView>::Get()->UnloadAll();
    }

    bool LoadConvexHullAsset(const std::string& path)
    {
        return AssetService<ConvexHullView>::Get()->Load(path);
    }

    bool LoadConvexHullAssets(std::span<const std::string> paths)
    {
        return AssetService<ConvexHullView>::Get()->Load(paths);
    }

    bool UnloadConvexHullAssets(const std::string& path)
    {
        return AssetService<ConvexHullView>::Get()->Unload(path);
    }

    void UnloadAllConvexHullAsset()
    {
        AssetService<ConvexHullView>::Get()->UnloadAll();
    }

    bool LoadConcaveColliderAsset(const std::string& path)
    {
        return AssetService<ConcaveColliderView>::Get()->Load(path);
    }

    bool LoadConcaveColliderAssets(std::span<const std::string> paths)
    {
        return AssetService<ConcaveColliderView>::Get()->Load(paths);
    }

    bool UnloadConcaveColliderAsset(const std::string& path)
    {
        return AssetService<ConcaveColliderView>::Get()->Unload(path);
    }

    void UnloadAllConcaveColliderAssets()
    {
        AssetService<ConcaveColliderView>::Get()->UnloadAll();
    }

    bool LoadMeshAsset(const std::string& path)
    {
        return AssetService<MeshView>::Get()->Load(path);
    }

    bool LoadMeshAssets(std::span<const std::string> paths)
    {
        return AssetService<MeshView>::Get()->Load(paths);
    }

    bool UnloadMeshAsset(const std::string& path)
    {
        return AssetService<MeshView>::Get()->Unload(path);
    }

    void UnloadAllMeshAssets()
    {
        AssetService<MeshView>::Get()->UnloadAll();
    }

    bool LoadTextureAsset(const std::string& path)
    {
        return AssetService<TextureView>::Get()->Load(path);
    }

    bool LoadTextureAssets(std::span<const std::string> paths)
    {
        return AssetService<TextureView>::Get()->Load(paths);
    }

    bool UnloadTextureAsset(const std::string& path)
    {
        return AssetService<TextureView>::Get()->Unload(path);
    }

    void UnloadAllTextureAssets()
    {
        return AssetService<TextureView>::Get()->UnloadAll();
    }

    bool LoadCubeMapAsset(const CubeMapFaces& paths)
    {
        return AssetService<CubeMapView, CubeMapFaces>::Get()->Load(paths);
    }

    bool LoadCubeMapAssets(std::span<const CubeMapFaces> paths)
    {
        return AssetService<CubeMapView, CubeMapFaces>::Get()->Load(paths);
    }

    bool UnloadCubeMapAsset(const CubeMapFaces& paths)
    {
        return AssetService<CubeMapView, CubeMapFaces>::Get()->Unload(paths);
    }

    void UnloadAllCubeMapAssets()
    {
        return AssetService<CubeMapView, CubeMapFaces>::Get()->UnloadAll();
    }

} // namespace nc