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
namespace key
{
using namespace std::string_view_literals;
constexpr auto globalOptions = "globalOptions"sv;
constexpr auto outputDirectory = "outputDirectory"sv;
constexpr auto workingDirectory = "workingDirectory"sv;
constexpr auto defaultMeshOptions = "defaultMeshOptions"sv;
constexpr auto defaultCubeMapOptions = "defaultCubeMapOptions"sv;
constexpr auto defaultDiffuseTextureOptions = "defaultDiffuseTextureOptions"sv;
constexpr auto defaultNormalTextureOptions = "defaultNormalTextureOptions"sv;
constexpr auto defaultParticleTextureOptions = "defaultParticleTextureOptions"sv;
constexpr auto defaultEffectTextureOptions = "defaultEffectTextureOptions"sv;

constexpr auto audioClip = "audio-clip"sv;
constexpr auto convexHull = "convex-hull"sv;
constexpr auto cubeMap = "cube-map"sv;
constexpr auto mesh = "mesh"sv;
constexpr auto meshCollider = "mesh-collider"sv;
constexpr auto shapeKeyAnimation = "shapekey-animation"sv;
constexpr auto skeletalAnimation = "skeletal-animation"sv;
constexpr auto texture = "texture"sv;

constexpr auto diffuse = "diffuse"sv;
constexpr auto normal = "normal"sv;
constexpr auto particle = "particle"sv;
constexpr auto effect = "effect"sv;

constexpr auto sourcePath = "sourcePath"sv;
constexpr auto assetName = "assetName"sv;
constexpr auto assetNames = "assetNames"sv;
constexpr auto subResourceName = "subResourceName"sv;

constexpr auto options = "options"sv;
constexpr auto textureFormat = "textureFormat"sv;
constexpr auto generateMips = "generateMips"sv;
constexpr auto optimizeMesh = "optimizeMesh"sv;
}

const auto jsonAssetObjectTags = std::array<std::string, 1>{
    "texture"
};

const auto jsonAssetArrayTags = std::array<std::string, 7> {
    "audio-clip", "convex-hull", "cube-map", "mesh", "mesh-collider", "shapeKey-animation", "skeletal-animation"
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

auto GetDefaultTargetOptions(const nc::convert::GlobalManifestOptions& options,
                             nc::asset::AssetType type,
                             std::string_view subGroupName = "") -> nc::convert::TargetOptions
{
    using enum nc::asset::AssetType;
    switch (type)
    {
        case CubeMap:                         return options.defaultCubeMapOptions;
        case Mesh:                            return options.defaultMeshOptions;
        case Texture:
        {
            if (subGroupName == key::diffuse)  return options.defaultDiffuseTextureOptions;
            if (subGroupName == key::normal)   return options.defaultNormalTextureOptions;
            if (subGroupName == key::particle) return options.defaultParticleTextureOptions;
            if (subGroupName == key::effect)   return options.defaultEffectTextureOptions;
            [[fallthrough]];
        }
        default:                              return nc::convert::TargetOptions{};
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
        targets.emplace_back(
            RelativePathToIdentifier(path),
            std::string{path}
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
        case AssetType::ShapeKeyAnimation: return ReflectDefaults(GetDefaultShapeKeyAnimationPaths());
        case AssetType::SkeletalAnimation: return ReflectDefaults(GetDefaultSkeletalAnimationPaths());
        case AssetType::Texture:           return ReflectDefaults(GetDefaultTexturePaths());
        default:                           return {};
    }
}
} // anonymous namespace

namespace nc::convert
{
void from_json(const nlohmann::json& json, nc::convert::TargetOptions& options)
{
    if (json.contains(key::textureFormat))
    {
        options.textureFormat = ToTextureFormat(json.at(key::textureFormat));
    }

    options.generateMips = json.value(key::generateMips, false);
    options.optimizeMesh = json.value(key::optimizeMesh, false);
}

void from_json(const nlohmann::json& json, GlobalManifestOptions& options)
{
    options.outputDirectory = json.value(key::outputDirectory, "./");
    options.workingDirectory = json.value(key::workingDirectory, "./");
    options.defaultMeshOptions = json.value(key::defaultMeshOptions, TargetOptions{});
    options.defaultCubeMapOptions = json.value(key::defaultCubeMapOptions, TargetOptions{});
    options.defaultDiffuseTextureOptions = json.value(key::defaultDiffuseTextureOptions, TargetOptions{});
    options.defaultNormalTextureOptions = json.value(key::defaultNormalTextureOptions, TargetOptions{});
    options.defaultParticleTextureOptions = json.value(key::defaultParticleTextureOptions, TargetOptions{});
    options.defaultEffectTextureOptions = json.value(key::defaultEffectTextureOptions, TargetOptions{});
}

Manifest::Manifest(std::filesystem::path path)
    : m_path{std::move(path)}
{
    m_targets.emplace(nc::asset::AssetType::AudioClip, std::vector<nc::convert::Target>{});
    m_targets.emplace(nc::asset::AssetType::ConvexHull, std::vector<nc::convert::Target>{});
    m_targets.emplace(nc::asset::AssetType::CubeMap, std::vector<nc::convert::Target>{});
    m_targets.emplace(nc::asset::AssetType::Mesh, std::vector<nc::convert::Target>{});
    m_targets.emplace(nc::asset::AssetType::MeshCollider, std::vector<nc::convert::Target>{});
    m_targets.emplace(nc::asset::AssetType::ShapeKeyAnimation, std::vector<nc::convert::Target>{});
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
                path.generic_string() // normalize directory separators to '/'
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
    m_options = json.value(key::globalOptions, GlobalManifestOptions{});

    for (const auto& typeTag : ::jsonAssetObjectTags)
    {
        if (!json.contains(typeTag))
        {
            continue;
        }

        const auto type = ToAssetType(typeTag);
        for (const auto& [subGroup, assets] : json.at(typeTag).items())
        {
            const auto defaultOptions = GetDefaultTargetOptions(m_options, type, subGroup);
            for (const auto& asset : assets)
            {
                auto targetOptions = asset.value(key::options, defaultOptions);
                m_targets.at(type).emplace_back(
                    asset.at(key::sourcePath),
                    asset.at(key::assetName),
                    std::nullopt,
                    targetOptions
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
        const auto defaultOptions = GetDefaultTargetOptions(m_options, type);
        for (const auto& asset : json.at(typeTag))
        {
            const auto targetOptions = asset.value(key::options, defaultOptions);
            // Types that CanOutputMany support both single target (legacy) mode and multiple output mode.
            if (CanOutputMany(type))
            {
                // Multiple output mode
                if (asset.contains(key::assetNames))
                {
                    for (const auto& subResource : asset.at(key::assetNames))
                    {
                        m_targets.at(type).emplace_back(
                            asset.at(key::sourcePath),
                            subResource.at(key::assetName),
                            subResource.at(key::subResourceName),
                            targetOptions
                        );
                    }

                    continue;
                }
            }

            // Single target mode
            m_targets.at(type).emplace_back(
                asset.at(key::sourcePath),
                asset.at(key::assetName),
                std::nullopt,
                targetOptions
            );
        }
    }
}
} // namespace nc::convert
