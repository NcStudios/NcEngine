#include "BuildOrchestrator.h"
#include "Builder.h"
#include "BuildInstructions.h"
#include "Inspect.h"
#include "Target.h"
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
    nc::asset::AssetType::CubeMap,
    nc::asset::AssetType::ConcaveCollider,
    nc::asset::AssetType::HullCollider,
    nc::asset::AssetType::Mesh,
    nc::asset::AssetType::SkeletalAnimation,
    nc::asset::AssetType::Texture
};
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

    const auto instructions = BuildInstructions{m_config};
    LOG("--Building Assets--");
    for (auto type : assetTypes)
    {
        for (const auto& target : instructions.GetTargetsForType(type))
        {
            LOG("Building {}: {}", ToString(type), target.destinationPath.string());
            if (!m_builder->Build(type, target))
            {
                LOG("Failed building: {}", target.destinationPath.string());
            }
        }
    }
}
} // namespace nc::convert
