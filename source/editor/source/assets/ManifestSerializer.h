#pragma once

#include "Asset.h"

#include <vector>

namespace nc::editor
{
class AssetManifest;

struct ManifestData
{
    std::vector<Asset> audioClips;
    std::vector<Asset> concaveColliders;
    std::vector<Asset> hullColliders;
    std::vector<Asset> meshes;
    std::vector<Asset> textures;
    std::vector<Asset> skyboxes;
    bool readSuccessfully;
};

/** Create a ManifestData object from a manifest file. */
auto ReadManifest(const std::filesystem::path& manifestPath) -> ManifestData;

/** Write a ManifestData object to a manifest file. */
void WriteManifest(const std::filesystem::path& manifestPath, const AssetManifest& manifest);
} // namespace nc::editor
