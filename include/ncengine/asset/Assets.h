#pragma once

#include "ncengine/asset/AssetViews.h"

namespace nc
{
/** Asset flags are used to pass parameters to the asset loading/unloading functions. */
using asset_flags_type = uint64_t;
struct AssetFlags
{
    static constexpr asset_flags_type None                 = 0b00000000;
    static constexpr asset_flags_type TextureTypeImage     = 0b00000001;
    static constexpr asset_flags_type TextureTypeNormalMap = 0b00000010;
};

/** Assets must be loaded before dependent objects are created and should be unloaded only
 *  when they are no longer in use.
 * 
 *  Paths should be relative to directories specified in the config. Passing true for isExternal
 *  allows paths to be absolute or relative to the executable. Duplicate loads are ignored. */

/** Supported file types: .nca */
bool LoadAudioClipAsset(const std::string& path, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool LoadAudioClipAssets(std::span<const std::string> paths, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool UnloadAudioClipAsset(const std::string& path, asset_flags_type flags = AssetFlags::None);
void UnloadAllAudioClipAssets(asset_flags_type flags = AssetFlags::None);
auto AcquireAudioClipAsset(const std::string& path) -> AudioClipView;

/** Supported file types: .nca */
bool LoadConcaveColliderAsset(const std::string& path, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool LoadConcaveColliderAssets(std::span<const std::string> paths, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool UnloadConcaveColliderAsset(const std::string& path, asset_flags_type flags = AssetFlags::None);
void UnloadAllConcaveColliderAssets(asset_flags_type flags = AssetFlags::None);

/** Supported file types: .nca */
bool LoadConvexHullAsset(const std::string& path, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool LoadConvexHullAssets(std::span<const std::string> paths, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool UnloadConvexHullAsset(const std::string& path, asset_flags_type flags = AssetFlags::None);
void UnloadAllConvexHullAssets(asset_flags_type flags = AssetFlags::None);

/** Supported file types: .nca 
*  @note Unloading textures invalidates all CubeMapViews. It is intended
*  to be done on scene change. */
bool LoadCubeMapAsset(const std::string& path, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool LoadCubeMapAssets(std::span<const std::string> paths, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool UnloadCubeMapAsset(const std::string& paths, asset_flags_type flags = AssetFlags::None);
void UnloadAllCubeMapAssets(asset_flags_type flags = AssetFlags::None);

/** Supported file types: .nca 
 *  @note Unloading meshes invalidates all MeshViews. It is intended
 *  to be done on scene change. */
bool LoadMeshAsset(const std::string& path, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool LoadMeshAssets(std::span<const std::string> paths, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool UnloadMeshAsset(const std::string& path, asset_flags_type flags = AssetFlags::None);
void UnloadAllMeshAssets(asset_flags_type flags = AssetFlags::None);

/** Supported file types: .nca 
 *  @note Unloading textures invalidates all TextureViews. It is intended
 *  to be done on scene change. */
bool LoadTextureAsset(const std::string& path, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool LoadTextureAssets(std::span<const std::string> paths, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool UnloadTextureAsset(const std::string& path, asset_flags_type flags = AssetFlags::None);
void UnloadAllTextureAssets(asset_flags_type flags = AssetFlags::None);
auto AcquireTextureAsset(const std::string& path) -> TextureView;

/** Supported file types: .nca 
 *  @note Unloading skeletal animations invalidates all SkeletalAnimations. It is intended
 *  to be done on scene change. */
bool LoadSkeletalAnimationAsset(const std::string& path, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool LoadSkeletalAnimationAssets(std::span<const std::string> paths, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool UnloadSkeletalAnimationAsset(const std::string& path, asset_flags_type flags = AssetFlags::None);
void UnloadAllSkeletalAnimationAssets(asset_flags_type flags = AssetFlags::None);

/** Supported file types: .ttf */
bool LoadFont(const FontInfo& font, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool LoadFonts(std::span<const FontInfo> font, bool isExternal = false, asset_flags_type flags = AssetFlags::None);
bool UnloadFont(const FontInfo& font, asset_flags_type flags = AssetFlags::None);
void UnloadAllFonts(asset_flags_type flags = AssetFlags::None);
auto AcquireFont(const FontInfo& font) -> FontView;
} // namespace nc
