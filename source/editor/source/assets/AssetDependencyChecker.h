#pragma once

#include "ncengine/ecs/Registry.h"

namespace nc::editor
{
class AssetManifest;

struct AssetDependencyChecker
{
    AssetDependencyChecker(const Registry* registry, const AssetManifest* manifest);
    void LogMissingDependencies();

    std::vector<std::string> missingDependencies;
    bool result;
};
} // namespace nc::editor
