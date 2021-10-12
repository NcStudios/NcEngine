#pragma once

#include "Ecs.h"

namespace nc::editor
{
    class AssetManifest;

    struct AssetDependencyChecker
    {
        AssetDependencyChecker(const registry_type* registry, const AssetManifest* manifest);
        void LogMissingDependencies();

        std::vector<std::string> missingDependencies;
        bool result;
    };
}