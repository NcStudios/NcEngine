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

    void LoadMesh(const std::string& path)
    {
        AssetService<MeshView>::Get()->Load(path);
    }

    void LoadMeshes(const std::vector<std::string>& paths)
    {
        AssetService<MeshView>::Get()->Load(paths);
    }

    void LoadTexture(const std::string& path)
    {
        AssetService<TextureView>::Get()->Load(path);
    }

    void LoadTextures(const std::vector<std::string>& paths)
    {
        AssetService<TextureView>::Get()->Load(paths);
    }
} // namespace nc