#include "Manifest.h"
#include "Target.h"
#include "utility/EnumExtensions.h"
#include "utility/Log.h"
#include "utility/Path.h"

#include "ncutility/NcError.h"
#include "nlohmann/json.hpp"

#include <fstream>

namespace
{
const auto jsonAssetArrayTags = std::array<std::string, 7> {
    "audio-clip", "concave-collider", "cube-map", "hull-collider", "mesh", "skeletal-animation", "texture"
};

struct GlobalManifestOptions
{
    std::filesystem::path outputDirectory;
    std::filesystem::path workingDirectory;
};

void from_json(const nlohmann::json& json, GlobalManifestOptions& options)
{
    options.outputDirectory = json.value("outputDirectory", "./");
    options.workingDirectory = json.value("workingDirectory", "./");
}

void ProcessOptions(GlobalManifestOptions& options, const std::filesystem::path& manifestPath)
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

auto BuildTarget(const std::string& assetName, const std::string& sourcePath, const std::filesystem::path& outputDirectory, const std::optional<std::string>& subResourceName = std::nullopt) -> nc::convert::Target
{
    auto target = nc::convert::Target
    {
        sourcePath,
        nc::convert::AssetNameToNcaPath(assetName, outputDirectory),
        subResourceName
    };

    if (!std::filesystem::is_regular_file(target.sourcePath))
    {
        throw nc::NcError("Invalid source file: ", target.sourcePath.string());
    }

    return target;
}

auto IsUpToDate(const nc::convert::Target& target) -> bool
{
    if (!std::filesystem::exists(target.destinationPath))
    {
        return false;
    }

    return std::filesystem::last_write_time(target.destinationPath) > std::filesystem::last_write_time(target.sourcePath);
}
} // anonymous namespace

namespace nc::convert
{
void ReadManifest(const std::filesystem::path& manifestPath, std::unordered_map<asset::AssetType, std::vector<Target>>& instructions)
{
    auto file = std::ifstream{manifestPath};
    if (!file.is_open())
    {
        throw nc::NcError{"Failed to open manifest: ", manifestPath.string()};
    }

    auto json = nlohmann::json::parse(file);
    auto options = json.value("globalOptions", ::GlobalManifestOptions{});
    ::ProcessOptions(options, manifestPath);

    for (const auto& typeTag : ::jsonAssetArrayTags)
    {
        if (!json.contains(typeTag))
        {
            continue;
        }

        const auto type = ToAssetType(typeTag);
        for (const auto& asset : json.at(typeTag))
        {
            // Types that CanOutputMany support both single target (legacy) mode and multiple output mode.
            if (CanOutputMany(type))
            {
                // // Multiple output mode
                if (asset.contains("assetNames"))
                {
                    for (const auto& subResource : asset.at("assetNames"))
                    {
                        auto target = BuildTarget(subResource.at("assetName"), asset.at("sourcePath"), options.outputDirectory, subResource.at("subResourceName"));
                        if (::IsUpToDate(target))
                        {
                            LOG("Up-to-date: {}", target.destinationPath.string());
                            continue;
                        }
                        instructions.at(type).push_back(std::move(target));
                    }
                    continue;
                }
            }

            // Single target mode
            auto target = BuildTarget(asset.at("assetName"), asset.at("sourcePath"), options.outputDirectory);
            if (::IsUpToDate(target))
            {
                LOG("Up-to-date: {}", target.destinationPath.string());
                continue;
            }
            instructions.at(type).push_back(std::move(target));
        }
    }
}
} // namespace nc::convert
