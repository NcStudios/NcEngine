#include "asset/Assets.h"
#include "AssetService.h"
#include "RuntimeTextureLoader.h"

#include "ncasset/Assets.h"

namespace nc::asset
{
bool LoadAudioClipAsset(const std::string& path)
{
    return AssetService<AudioClipView>::Get()->Load(path);
}

bool LoadAudioClipAssets(std::span<const std::string> paths)
{
    return AssetService<AudioClipView>::Get()->Load(paths);
}

bool UnloadAudioClipAsset(const std::string& path)
{
    return AssetService<AudioClipView>::Get()->Unload(path);
}

void UnloadAllAudioClipAssets()
{
    AssetService<AudioClipView>::Get()->UnloadAll();
}

auto AcquireAudioClipAsset(const std::string& path) -> AudioClipView
{
    return AssetService<AudioClipView>::Get()->Acquire(path);
}

auto AcquireAudioClipAsset(AssetId id) -> AudioClipView
{
    return AssetService<AudioClipView>::Get()->Acquire(id);
}

bool LoadConvexHullAsset(const std::string& path)
{
    return AssetService<ConvexHullView>::Get()->Load(path);
}

bool LoadConvexHullAssets(std::span<const std::string> paths)
{
    return AssetService<ConvexHullView>::Get()->Load(paths);
}

bool UnloadConvexHullAsset(const std::string& path)
{
    return AssetService<ConvexHullView>::Get()->Unload(path);
}

void UnloadAllConvexHullAssets()
{
    AssetService<ConvexHullView>::Get()->UnloadAll();
}

auto AcquireConvexHullAsset(const std::string& path) -> ConvexHullView
{
    return AssetService<ConvexHullView>::Get()->Acquire(path);
}

auto AcquireConvexHullAsset(AssetId id) -> ConvexHullView
{
    return AssetService<ConvexHullView>::Get()->Acquire(id);
}

bool LoadMeshColliderAsset(const std::string& path)
{
    return AssetService<MeshColliderView>::Get()->Load(path);
}

bool LoadMeshColliderAssets(std::span<const std::string> paths)
{
    return AssetService<MeshColliderView>::Get()->Load(paths);
}

bool UnloadMeshColliderAsset(const std::string& path)
{
    return AssetService<MeshColliderView>::Get()->Unload(path);
}

void UnloadAllMeshColliderAssets()
{
    AssetService<MeshColliderView>::Get()->UnloadAll();
}

auto AcquireMeshColliderAsset(const std::string& path) -> MeshColliderView
{
    return AssetService<MeshColliderView>::Get()->Acquire(path);
}

auto AcquireMeshColliderAsset(AssetId id) -> MeshColliderView
{
    return AssetService<MeshColliderView>::Get()->Acquire(id);
}

bool LoadCubeMapAsset(const std::string& path)
{
    return AssetService<CubeMapView>::Get()->Load(path);
}

bool LoadCubeMapAssets(std::span<const std::string> paths)
{
    return AssetService<CubeMapView>::Get()->Load(paths);
}

bool UnloadCubeMapAsset(const std::string& path)
{
    return AssetService<CubeMapView>::Get()->Unload(path);
}

void UnloadAllCubeMapAssets()
{
    return AssetService<CubeMapView>::Get()->UnloadAll();
}

auto AcquireCubeMapAsset(const std::string& path) -> CubeMapView
{
    return AssetService<CubeMapView>::Get()->Acquire(path);
}

auto AcquireCubeMapAsset(AssetId id) -> CubeMapView
{
    return AssetService<CubeMapView>::Get()->Acquire(id);
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

auto AcquireMeshAsset(const std::string& path) -> MeshView
{
    return AssetService<MeshView>::Get()->Acquire(path);
}

auto AcquireMeshAsset(AssetId id) -> MeshView
{
    return AssetService<MeshView>::Get()->Acquire(id);
}

bool LoadSkeletalAnimationAsset(const std::string& path)
{
    return AssetService<SkeletalAnimationView>::Get()->Load(path);
}

bool LoadSkeletalAnimationAssets(std::span<const std::string> paths)
{
    return AssetService<SkeletalAnimationView>::Get()->Load(paths);
}

bool UnloadSkeletalAnimationAsset(const std::string& path)
{
    return AssetService<SkeletalAnimationView>::Get()->Unload(path);
}

void UnloadAllSkeletalAnimationAssets()
{
    return AssetService<SkeletalAnimationView>::Get()->UnloadAll();
}

auto AcquireSkeletalAnimationAsset(const std::string& path) -> SkeletalAnimationView
{
    return AssetService<SkeletalAnimationView>::Get()->Acquire(path);
}

auto AcquireSkeletalAnimationAsset(AssetId id) -> SkeletalAnimationView
{
    return AssetService<SkeletalAnimationView>::Get()->Acquire(id);
}

bool LoadTextureAsset(const std::string& path)
{
    return AssetService<TextureView>::Get()->Load(path);
}

bool LoadTextureAssets(std::span<const std::string> paths)
{
    return AssetService<TextureView>::Get()->Load(paths);
}

bool LoadTextureFromMemory(const std::string& key, Texture texture)
{
    return RuntimeTextureLoaderService::Get()->LoadFromMemory(key, std::move(texture));
}

bool LoadTextureFromRGBA(const std::string& key,
                         std::span<const uint8_t> rgbaData,
                         uint32_t width,
                         uint32_t height,
                         TextureFormat format)
{
    return RuntimeTextureLoaderService::Get()->LoadFromRGBA(key, rgbaData, width, height, format);
}

bool UnloadTextureAsset(const std::string& path)
{
    return AssetService<TextureView>::Get()->Unload(path);
}

void UnloadAllTextureAssets()
{
    return AssetService<TextureView>::Get()->UnloadAll();
}

auto AcquireTextureAsset(const std::string& path) -> TextureView
{
    return AssetService<TextureView>::Get()->Acquire(path);
}

auto AcquireTextureAsset(AssetId id) -> TextureView
{
    return AssetService<TextureView>::Get()->Acquire(id);
}

bool LoadFont(const FontInfo& font)
{
    return AssetService<FontView, FontInfo>::Get()->Load(font);
}

bool LoadFonts(std::span<const FontInfo> fonts)
{
    return AssetService<FontView, FontInfo>::Get()->Load(fonts);
}

auto AcquireFont(const FontInfo& font) -> FontView
{
    return AssetService<FontView, FontInfo>::Get()->Acquire(font);
}

bool UnloadFont(const FontInfo& font)
{
    return AssetService<FontView, FontInfo>::Get()->Unload(font);
}

void UnloadAllFonts()
{
    return AssetService<FontView, FontInfo>::Get()->UnloadAll();
}
} // namespace nc::asset
