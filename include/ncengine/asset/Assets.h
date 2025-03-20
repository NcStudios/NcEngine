#pragma once

#include "ncasset/AssetType.h"
#include "ncengine/asset/AssetViews.h"

namespace nc::asset
{
/** Assets must be loaded before dependent objects are created and should be unloaded only
 *  when they are no longer in use.
 * 
 *  Paths should be relative to directories specified in the config. Passing true for isExternal
 *  allows paths to be absolute or relative to the executable. Duplicate loads are ignored. */

/** Supported file types: .nca */
bool LoadAudioClipAsset(const std::string& path);
bool LoadAudioClipAssets(std::span<const std::string> paths);
bool UnloadAudioClipAsset(const std::string& path);
void UnloadAllAudioClipAssets();
auto AcquireAudioClipAsset(const std::string& path) -> AudioClipView;
auto AcquireAudioClipAsset(AssetId id) -> AudioClipView;

/** Supported file types: .nca */
bool LoadMeshColliderAsset(const std::string& path);
bool LoadMeshColliderAssets(std::span<const std::string> paths);
bool UnloadMeshColliderAsset(const std::string& path);
void UnloadAllMeshColliderAssets();
auto AcquireMeshColliderAsset(const std::string& path) -> MeshColliderView;
auto AcquireMeshColliderAsset(AssetId id) -> MeshColliderView;

/** Supported file types: .nca */
bool LoadConvexHullAsset(const std::string& path);
bool LoadConvexHullAssets(std::span<const std::string> paths);
bool UnloadConvexHullAsset(const std::string& path);
void UnloadAllConvexHullAssets();
auto AcquireConvexHullAsset(const std::string& path) -> ConvexHullView;
auto AcquireConvexHullAsset(AssetId id) -> ConvexHullView;

/** Supported file types: .nca 
*  @note Unloading textures invalidates all CubeMapViews. It is intended
*  to be done on scene change. */
bool LoadCubeMapAsset(const std::string& path);
bool LoadCubeMapAssets(std::span<const std::string> paths);
bool UnloadCubeMapAsset(const std::string& paths);
void UnloadAllCubeMapAssets();
auto AcquireCubeMapAsset(const std::string& path) -> CubeMapView;
auto AcquireCubeMapAsset(AssetId id) -> CubeMapView;

/** Supported file types: .nca 
 *  @note Unloading meshes invalidates all MeshViews. It is intended
 *  to be done on scene change. */
bool LoadMeshAsset(const std::string& path);
bool LoadMeshAssets(std::span<const std::string> paths);
bool UnloadMeshAsset(const std::string& path);
void UnloadAllMeshAssets();
auto AcquireMeshAsset(const std::string& path) -> MeshView;
auto AcquireMeshAsset(AssetId id) -> MeshView;

/** Supported file types: .nca 
 *  @note Unloading textures invalidates all TextureViews. It is intended
 *  to be done on scene change. */

bool LoadTextureAsset(const std::string& path, AssetSubtype subtype = AssetSubtype::DiffuseTexture);
bool LoadTextureAssets(std::span<const std::string> paths, AssetSubtype subtype = AssetSubtype::DiffuseTexture);
bool LoadTextureAssets(std::span<const std::string> paths, std::span<const AssetSubtype> subtypes);
bool UnloadTextureAsset(const std::string& path);
void UnloadAllTextureAssets();
auto AcquireTextureAsset(const std::string& path) -> TextureView;
auto AcquireTextureAsset(AssetId id) -> TextureView;

/** Supported file types: .nca 
 *  @note Unloading skeletal animations invalidates all SkeletalAnimations. It is intended
 *  to be done on scene change. */
bool LoadSkeletalAnimationAsset(const std::string& path);
bool LoadSkeletalAnimationAssets(std::span<const std::string> paths);
bool UnloadSkeletalAnimationAsset(const std::string& path);
void UnloadAllSkeletalAnimationAssets();
auto AcquireSkeletalAnimationAsset(const std::string& path) -> SkeletalAnimationView;
auto AcquireSkeletalAnimationAsset(AssetId id) -> SkeletalAnimationView;

/** Supported file types: .ttf */
bool LoadFont(const FontInfo& font);
bool LoadFonts(std::span<const FontInfo> font);
bool UnloadFont(const FontInfo& font);
void UnloadAllFonts();
auto AcquireFont(const FontInfo& font) -> FontView;
} // namespace nc::asset
