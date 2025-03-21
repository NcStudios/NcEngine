#include "Manifest.h"
#include "Target.h"
#include "utility/EnumExtensions.h"
#include "utility/Log.h"
#include "utility/Path.h"

#include "ncasset/DefaultAssets.h"
#include "ncutility/NcError.h"
#include "nlohmann/json.hpp"

#include <fstream>

namespace
{
const auto jsonAssetObjectTags = std::array<std::string, 1>{
    "texture"
};

const auto jsonAssetArrayTags = std::array<std::string, 7> {
    "audio-clip", "convex-hull", "cube-map", "mesh", "mesh-collider", "skeletal-animation"
};

void ProcessOptions(nc::convert::GlobalManifestOptions& options,
                    const std::filesystem::path& manifestPath)
{
    options.outputDirectory.make_preferred();
    options.workingDirectory.make_preferred();
    auto&& parentPath = std::filesystem::absolute(manifestPath.parent_path());

    if (options.workingDirectory.is_relative())
    {
        options.workingDirectory = parentPath / options.workingDirectory;
    }

    if (options.outputDirectory.is_relative())
    {
        options.outputDirectory = parentPath / options.outputDirectory;
    }

    LOG("Setting working directory: {}", options.workingDirectory.string());
    std::filesystem::current_path(options.workingDirectory);

    if(!std::filesystem::exists(options.outputDirectory))
    {
        LOG("Creating directory: {}", options.outputDirectory.string());
        if(!std::filesystem::create_directories(options.outputDirectory))
        {
            throw nc::NcError("Failed to create output directory: ", options.outputDirectory.string());
        }
    }
}

auto IsUpToDate(const nc::convert::Target& target) -> bool
{
    if (!std::filesystem::exists(target.destinationPath))
    {
        return false;
    }

    return std::filesystem::last_write_time(target.destinationPath) > std::filesystem::last_write_time(target.sourcePath);
}

auto ToAssetSubtype(std::string_view str) -> nc::asset::AssetSubtype
{
    using enum nc::asset::AssetSubtype;
    if (str == "normal")   return NormalTexture;
    if (str == "diffuse")  return DiffuseTexture;
    if (str == "particle") return DiffuseTexture;
    if (str == "effect")   return DiffuseTexture;
    return None;
}

auto AssetNameToRelativePath(const std::filesystem::path& name) -> std::filesystem::path
{
    auto strippedPath = std::filesystem::path{};
    for (auto it = ++name.begin(); it != name.end(); ++it)
    {
        strippedPath /= *it;
    }

    strippedPath.replace_extension(".nca");
    return strippedPath;
}

auto RelativePathToIdentifier(std::filesystem::path path) -> std::string
{
    path.replace_extension("");
    auto it = path.begin();
    auto end = path.end();

    auto string = std::string{it->string()};
    for (++it; it != end; ++it)
    {
        string.append("_");
        string.append(it->string());
    }

    return string;
}

auto ReflectDefaults(std::span<const std::string_view> defaultPaths) -> std::vector<nc::convert::ReflectedTarget>
{
    auto targets = std::vector<nc::convert::ReflectedTarget>{};
    targets.reserve(defaultPaths.size());
    for (const auto& path : defaultPaths)
    {
        const auto extensionPos = path.find('.');
        targets.emplace_back(
            std::string{path.substr(0, extensionPos)},
            std::string{path},
            nc::asset::AssetSubtype::None
        );
    }

    return targets;
}

auto ReflectDefaults(std::span<const std::string_view> defaultPaths,
                     std::span<const nc::asset::AssetSubtype> subtypes) -> std::vector<nc::convert::ReflectedTarget>
{
    auto targets = std::vector<nc::convert::ReflectedTarget>{};
    targets.reserve(defaultPaths.size());
    for (const auto [path, subtype] : std::views::zip(defaultPaths, subtypes))
    {
        const auto extensionPos = path.find('.');
        targets.emplace_back(
            std::string{path.substr(0, extensionPos)},
            std::string{path},
            subtype
        );
    }

    return targets;
}

auto ReflectDefaults(nc::asset::AssetType type) -> std::vector<nc::convert::ReflectedTarget>
{
    using namespace nc::asset;
    switch (type)
    {
        case AssetType::AudioClip:         return ReflectDefaults(GetDefaultAudioClipPaths());
        case AssetType::ConvexHull:        return ReflectDefaults(GetDefaultConvexHullPaths());
        case AssetType::CubeMap:           return ReflectDefaults(GetDefaultCubeMapPaths());
        case AssetType::Mesh:              return ReflectDefaults(GetDefaultMeshPaths());
        case AssetType::MeshCollider:      return ReflectDefaults(GetDefaultMeshColliderPaths());
        case AssetType::SkeletalAnimation: return ReflectDefaults(GetDefaultSkeletalAnimationPaths());
        case AssetType::Texture:           return ReflectDefaults(GetDefaultTexturePaths(), GetDefaultTextureSubtypes());
        default:                           return {};
    }
}
} // anonymous namespace

namespace nc::convert
{
void from_json(const nlohmann::json& json, GlobalManifestOptions& options)
{
    options.outputDirectory = json.value("outputDirectory", "./");
    options.workingDirectory = json.value("workingDirectory", "./");
}

void from_json(const nlohmann::json& json, nc::convert::TargetOptions& options)
{
    options.optimizeMesh = json.value("optimizeMesh", false);
}

Manifest::Manifest(std::filesystem::path path)
    : m_path{std::move(path)}
{
    m_targets.emplace(nc::asset::AssetType::AudioClip, std::vector<nc::convert::Target>{});
    m_targets.emplace(nc::asset::AssetType::ConvexHull, std::vector<nc::convert::Target>{});
    m_targets.emplace(nc::asset::AssetType::CubeMap, std::vector<nc::convert::Target>{});
    m_targets.emplace(nc::asset::AssetType::Mesh, std::vector<nc::convert::Target>{});
    m_targets.emplace(nc::asset::AssetType::MeshCollider, std::vector<nc::convert::Target>{});
    m_targets.emplace(nc::asset::AssetType::SkeletalAnimation, std::vector<nc::convert::Target>{});
    m_targets.emplace(nc::asset::AssetType::Texture, std::vector<nc::convert::Target>{});
    ReadManifest(m_path);
}

void Manifest::PrepareForBuild()
{
    ::ProcessOptions(m_options, m_path);
    for (auto& [_, targetList] : m_targets)
    {
        for (auto& target : targetList)
        {
            if (!std::filesystem::is_regular_file(target.sourcePath))
            {
                throw nc::NcError("Invalid source file: ", target.sourcePath.string());
            }

            target.destinationPath = AssetNameToNcaPath(target.destinationPath, m_options.outputDirectory);
        }

        std::erase_if(
            targetList,
            [](const Target& target) {
                if (::IsUpToDate(target))
                {
                    LOG("Up-to-date: {}", target.destinationPath.string());
                    return true;
                }

                return false;
            }
        );
    }
}

auto Manifest::ExtractTargetsForBuild() -> TargetMap
{
    PrepareForBuild();
    return std::move(m_targets);
}

auto Manifest::GetTargetsForSourceGeneration() -> ReflectedTargetMap
{
    auto out = ReflectedTargetMap{};
    for (const auto& [type, targetList] : m_targets)
    {
        auto reflectedTargets = ReflectDefaults(type);
        reflectedTargets.reserve(reflectedTargets.size() + targetList.size());
        for (const auto& target : targetList)
        {
            auto path = AssetNameToRelativePath(target.destinationPath);
            auto name = RelativePathToIdentifier(path);
            reflectedTargets.emplace_back(
                std::move(name),
                path.generic_string(), // normalize directory separators to '/'
                target.options.subtype
            );
        }

        out.emplace(type, std::move(reflectedTargets));
    }

    return out;
}

void Manifest::ReadManifest(const std::filesystem::path& path)
{
    auto file = std::ifstream{path};
    if (!file.is_open())
    {
        throw nc::NcError{"Failed to open manifest: ", path.string()};
    }

    auto json = nlohmann::json::parse(file);
    m_options = json.value("globalOptions", GlobalManifestOptions{});

    for (const auto& typeTag : ::jsonAssetObjectTags)
    {
        if (!json.contains(typeTag))
        {
            continue;
        }

        const auto type = ToAssetType(typeTag);
        for (const auto& [subtypeName, assets] : json.at(typeTag).items())
        {
            for (const auto& asset : assets)
            {
                m_targets.at(type).emplace_back(
                    asset.at("sourcePath"),
                    asset.at("assetName"),
                    std::nullopt,
                    TargetOptions{
                        .subtype = ToAssetSubtype(subtypeName)
                    }
                );
            }
        }
    }

    for (const auto& typeTag : ::jsonAssetArrayTags)
    {
        if (!json.contains(typeTag))
        {
            continue;
        }

        const auto type = ToAssetType(typeTag);
        for (const auto& asset : json.at(typeTag))
        {
            const auto targetOptions = asset.value("options", TargetOptions{});
            // Types that CanOutputMany support both single target (legacy) mode and multiple output mode.
            if (CanOutputMany(type))
            {
                // Multiple output mode
                if (asset.contains("assetNames"))
                {
                    for (const auto& subResource : asset.at("assetNames"))
                    {
                        m_targets.at(type).emplace_back(
                            asset.at("sourcePath"),
                            subResource.at("assetName"),
                            subResource.at("subResourceName"),
                            targetOptions
                        );
                    }

                    continue;
                }
            }

            // Single target mode
            m_targets.at(type).emplace_back(
                asset.at("sourcePath"),
                asset.at("assetName"),
                std::nullopt,
                targetOptions
            );
        }
    }
}
} // namespace nc::convert
