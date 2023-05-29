#include "asset/Assets.h"
#include "AssetService.h"

namespace nc
{
    bool LoadAudioClipAsset(const std::string& path, bool isExternal, asset_flags_type flags)
    {
        return AssetService<AudioClipView>::Get()->Load(path, isExternal, flags);
    }

    bool LoadAudioClipAssets(std::span<const std::string> paths, bool isExternal, asset_flags_type flags)
    {
        return AssetService<AudioClipView>::Get()->Load(paths, isExternal, flags);
    }

    bool UnloadAudioClipAsset(const std::string& path, asset_flags_type flags)
    {
        return AssetService<AudioClipView>::Get()->Unload(path, flags);
    }

    void UnloadAllAudioClipAssets(asset_flags_type flags)
    {
        AssetService<AudioClipView>::Get()->UnloadAll(flags);
    }

    bool LoadConvexHullAsset(const std::string& path, bool isExternal, asset_flags_type flags)
    {
        return AssetService<ConvexHullView>::Get()->Load(path, isExternal, flags);
    }

    bool LoadConvexHullAssets(std::span<const std::string> paths, bool isExternal, asset_flags_type flags)
    {
        return AssetService<ConvexHullView>::Get()->Load(paths, isExternal, flags);
    }

    bool UnloadConvexHullAsset(const std::string& path, asset_flags_type flags)
    {
        return AssetService<ConvexHullView>::Get()->Unload(path, flags);
    }

    void UnloadAllConvexHullAssets(asset_flags_type flags)
    {
        AssetService<ConvexHullView>::Get()->UnloadAll(flags);
    }

    bool LoadConcaveColliderAsset(const std::string& path, bool isExternal, asset_flags_type flags)
    {
        return AssetService<ConcaveColliderView>::Get()->Load(path, isExternal, flags);
    }

    bool LoadConcaveColliderAssets(std::span<const std::string> paths, bool isExternal, asset_flags_type flags)
    {
        return AssetService<ConcaveColliderView>::Get()->Load(paths, isExternal, flags);
    }

    bool UnloadConcaveColliderAsset(const std::string& path, asset_flags_type flags)
    {
        return AssetService<ConcaveColliderView>::Get()->Unload(path, flags);
    }

    void UnloadAllConcaveColliderAssets(asset_flags_type flags)
    {
        AssetService<ConcaveColliderView>::Get()->UnloadAll(flags);
    }

    bool LoadMeshAsset(const std::string& path, bool isExternal, asset_flags_type flags)
    {
        return AssetService<MeshView>::Get()->Load(path, isExternal, flags);
    }

    bool LoadMeshAssets(std::span<const std::string> paths, bool isExternal, asset_flags_type flags)
    {
        return AssetService<MeshView>::Get()->Load(paths, isExternal, flags);
    }

    bool UnloadMeshAsset(const std::string& path, asset_flags_type flags)
    {
        return AssetService<MeshView>::Get()->Unload(path, flags);
    }

    void UnloadAllMeshAssets(asset_flags_type flags)
    {
        AssetService<MeshView>::Get()->UnloadAll(flags);
    }

    bool LoadTextureAsset(const std::string& path, bool isExternal, asset_flags_type flags)
    {
        return AssetService<TextureView>::Get()->Load(path, isExternal, flags);
    }

    bool LoadTextureAssets(std::span<const std::string> paths, bool isExternal, asset_flags_type flags)
    {
        return AssetService<TextureView>::Get()->Load(paths, isExternal, flags);
    }

    bool UnloadTextureAsset(const std::string& path, asset_flags_type flags)
    {
        return AssetService<TextureView>::Get()->Unload(path, flags);
    }

    void UnloadAllTextureAssets(asset_flags_type flags)
    {
        return AssetService<TextureView>::Get()->UnloadAll(flags);
    }

    bool LoadCubeMapAsset(const std::string& path, bool isExternal, asset_flags_type flags)
    {
        return AssetService<CubeMapView>::Get()->Load(path, isExternal, flags);
    }

    bool LoadCubeMapAssets(std::span<const std::string> paths, bool isExternal, asset_flags_type flags)
    {
        return AssetService<CubeMapView>::Get()->Load(paths, isExternal, flags);
    }

    bool UnloadCubeMapAsset(const std::string& path, asset_flags_type flags)
    {
        return AssetService<CubeMapView>::Get()->Unload(path, flags);
    }

    void UnloadAllCubeMapAssets(asset_flags_type flags)
    {
        return AssetService<CubeMapView>::Get()->UnloadAll(flags);
    }

} // namespace nc