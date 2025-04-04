#pragma once

#include "Target.h"

#include "ncasset/AssetType.h"

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace nc::convert
{
using TargetMap = std::unordered_map<asset::AssetType, std::vector<Target>>;
using ReflectedTargetMap = std::unordered_map<asset::AssetType, std::vector<ReflectedTarget>>;

/** @brief Manifest file options. */
struct GlobalManifestOptions
{
    std::filesystem::path outputDirectory = "./";
    std::filesystem::path workingDirectory = "./";
    TargetOptions defaultMeshOptions = TargetOptions{};
    TargetOptions defaultCubeMapOptions = TargetOptions{};
    TargetOptions defaultDiffuseTextureOptions = TargetOptions{};
    TargetOptions defaultNormalTextureOptions = TargetOptions{};
    TargetOptions defaultParticleTextureOptions = TargetOptions{};
    TargetOptions defaultEffectTextureOptions = TargetOptions{};
};

/** @brief Manifest json parser. */
class Manifest
{
    public:
        /** @brief Parse targets from a manifest. */
        explicit Manifest(std::filesystem::path path);

        /** @brief Perform build-specific target transformations/filtering and extract targets that required building. */
        auto ExtractTargetsForBuild() -> TargetMap;

        /** @brief Perform generation-specific transformations and get a map describing all targets, including engine defaults. */
        auto GetTargetsForSourceGeneration() -> ReflectedTargetMap;

    private:
        std::filesystem::path m_path;
        GlobalManifestOptions m_options;
        TargetMap m_targets;

        void ReadManifest(const std::filesystem::path& path);
        void PrepareForBuild();
};
} // namespace nc::convert
