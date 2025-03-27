#include "BuildOrchestrator.h"
#include "Builder.h"
#include "BuildInstructions.h"
#include "Inspect.h"
#include "Target.h"
#include "code_gen/PrefabGenerator.h"
#include "utility/EnumExtensions.h"
#include "utility/Log.h"

#include "ncasset/AssetType.h"
#include "ncutility/NcError.h"

#include <array>
#include <fstream>

namespace
{
constexpr auto assetTypes = std::array<nc::asset::AssetType, 7>{
    nc::asset::AssetType::AudioClip,
    nc::asset::AssetType::ConvexHull,
    nc::asset::AssetType::CubeMap,
    nc::asset::AssetType::Mesh,
    nc::asset::AssetType::MeshCollider,
    nc::asset::AssetType::SkeletalAnimation,
    nc::asset::AssetType::Texture
};

auto GetOptionText(nc::asset::AssetType type, const nc::convert::Target& target) -> std::string_view
{
    if (type == nc::asset::AssetType::Mesh)
    {
        return target.options.optimizeMesh
            ? std::string_view{"[optimizeMesh: true]"}
            : std::string_view{"[optimizeMesh: false]"};
    }

    return std::string_view{};
}
}

namespace nc::convert
{
BuildOrchestrator::BuildOrchestrator(Config config)
    : m_config{std::move(config)},
      m_builder{std::make_unique<Builder>()}
{
}

BuildOrchestrator::~BuildOrchestrator() noexcept = default;

void BuildOrchestrator::RunBuild()
{
    if (m_config.mode == OperationMode::Inspect)
    {
        Inspect(m_config.targetPath.value());
        return;
    }

    if(!std::filesystem::exists(m_config.outputDirectory))
    {
        LOG("Creating directory: {}", m_config.outputDirectory.string());
        if(!std::filesystem::create_directories(m_config.outputDirectory))
        {
            throw NcError("Failed to create output directory: ", m_config.outputDirectory.string());
        }
    }

    if (m_config.mode == OperationMode::GenerateSource)
    {
        LOG("Generating source code at {}", m_config.outputDirectory.string());
        GeneratePrefabFile(m_config.manifestPath.value(), m_config.outputDirectory, m_config.rootNamespace);
        return;
    }

    const auto instructions = BuildInstructions{m_config};
    LOG("--Building Assets--");
    for (auto type : assetTypes)
    {
        for (const auto& target : instructions.GetTargetsForType(type))
        {
            LOG("Building {}: {} ({})", ToString(type), target.destinationPath.string(), GetOptionText(type, target));
            if (!m_builder->Build(type, target))
            {
                LOG("Failed building: {}", target.destinationPath.string());
            }
        }
    }
}
} // namespace nc::convert
