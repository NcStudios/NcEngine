#include "BuildInstructions.h"
#include "Config.h"
#include "Manifest.h"
#include "Target.h"
#include "utility/Log.h"
#include "utility/Path.h"

#include "ncutility/NcError.h"

namespace
{
auto BuildTargetMap() -> std::unordered_map<nc::asset::AssetType, std::vector<nc::convert::Target>>
{
    auto out = std::unordered_map<nc::asset::AssetType, std::vector<nc::convert::Target>>{};
    out.emplace(nc::asset::AssetType::AudioClip, std::vector<nc::convert::Target>{});
    out.emplace(nc::asset::AssetType::CubeMap, std::vector<nc::convert::Target>{});
    out.emplace(nc::asset::AssetType::ConcaveCollider, std::vector<nc::convert::Target>{});
    out.emplace(nc::asset::AssetType::HullCollider, std::vector<nc::convert::Target>{});
    out.emplace(nc::asset::AssetType::Mesh, std::vector<nc::convert::Target>{});
    out.emplace(nc::asset::AssetType::SkeletalAnimation, std::vector<nc::convert::Target>{});
    out.emplace(nc::asset::AssetType::Texture, std::vector<nc::convert::Target>{});
    return out;
}
} // anonymous namespace

namespace nc::convert
{
BuildInstructions::BuildInstructions(const Config& config)
    : m_instructions{::BuildTargetMap()}
{
    ReadTargets(config);
}

auto BuildInstructions::GetTargetsForType(asset::AssetType type) const -> const std::vector<Target>&
{
    return m_instructions.at(type);
}

void BuildInstructions::ReadTargets(const Config& config)
{
    LOG("--Generating Build Targets--");
    switch (config.mode)
    {
        case OperationMode::SingleTarget:
        {
            LOG("Running in single target mode");
            auto& collection = m_instructions.at(config.targetType.value());
            auto sourcePath = config.targetPath.value();
            auto destinationPath = AssetNameToNcaPath(config.assetName.value(), config.outputDirectory);
            LOG("Adding build target: {} -> {}", sourcePath.string(), destinationPath.string());
            collection.emplace_back(std::move(sourcePath), std::move(destinationPath));
            break;
        }
        case OperationMode::Manifest:
        {
            LOG("Running in manifest mode");
            ReadManifest(config.manifestPath.value(), m_instructions);
            break;
        }
        default:
        {
            LOG("Unknown OperationMode. Not reading targets");
            break;
        }
    }
}
} // namespace nc::convert
