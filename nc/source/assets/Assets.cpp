#include "Assets.h"
#include "AssetService.h"

namespace nc
{
    void LoadSoundClipAsset(const std::string& path)
    {
        AssetService<SoundClipView>::Get()->Load(path);
    }

    void LoadConvexHullAsset(const std::string& path)
    {
        AssetService<ConvexHullView>::Get()->Load(path);
    }

    void LoadConcaveColliderAsset(const std::string& path)
    {
        AssetService<ConcaveColliderView>::Get()->Load(path);
    }

    void LoadMeshAsset(const std::string& path)
    {
        AssetService<MeshView>::Get()->Load(path);
    }

    void LoadMeshAssets(const std::vector<std::string>& paths)
    {
        AssetService<MeshView>::Get()->Load(paths);
    }

    void LoadTextureAsset(const std::string& path)
    {
        AssetService<TextureView>::Get()->Load(path);
    }

    void LoadTextureAssets(const std::vector<std::string>& paths)
    {
        AssetService<TextureView>::Get()->Load(paths);
    }
} // namespace nc