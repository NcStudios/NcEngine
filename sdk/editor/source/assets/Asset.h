#pragma once

#include "asset/AssetId.h"

#include <filesystem>
#include <optional>
#include <string>

namespace nc::editor
{
/** A project dependency that must be loaded at runtime. */
struct Asset
{
    /** The file path's stem. Used for displaying in UI controls. */
    std::string name;

    /** The path to the asset. */
    std::filesystem::path sourcePath;

    /** The path to the .nca representation of the asset. Used for
     *  meshes, skyboxes and colliders. */
    std::optional<std::filesystem::path> ncaPath;

    /** @todo add lastWriteTime and automatic rebuilding of assets */
    //std::filesystem::file_time_type lastWriteTime;
};

/** Create an Asset object from a path and type. This will not build any .nca
 *  files or load the asset. */
auto CreateAsset(const std::filesystem::path& assetPath, AssetType type) -> Asset;

/** Run the asset builder to create an nca file for the specified asset source
 *  file. Only asset types requiring an nca should be specified. */
bool BuildNcaFile(const std::filesystem::path& assetPath, AssetType type);

/** Load an asset into the appropriate system. For types requiring an nca file,
 *  it should be built before loading. */
bool LoadAsset(const Asset& asset, AssetType type);

/** Load resources used by the engine. These are also used as fallbacks when
 *  loading a resource fails. */
void LoadDefaultAssets();

/** Check if a file's extension is valid for an asset type. */
bool HasValidExtensionForAssetType(const std::filesystem::path& assetPath, AssetType type);

/** Check if an asset type uses an .nca file. */
bool RequiresNcaFile(AssetType type);
} // namespace nc::editor
