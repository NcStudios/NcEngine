#pragma once

#include "ncasset/AssetType.h"

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace nc::convert
{
struct Config;
struct Target;

/** @brief A collection of all targets that need to be built. */
class BuildInstructions
{
    public:
        /** @brief Construct a new BuildInstructions object from the data in Config. */
        BuildInstructions(const Config& config);

        /** @brief Get the collection of targets to build matching an AssetType. */
        auto GetTargetsForType(asset::AssetType type) const -> const std::vector<Target>&;

    private:
        std::unordered_map<asset::AssetType, std::vector<Target>> m_instructions;

        void ReadTargets(const Config& config);
};
} // namespace nc::convert
