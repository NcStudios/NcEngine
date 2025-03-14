#pragma once

#include "ncasset/AssetType.h"

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace nc::convert
{
struct Target;

void GeneratePrefabFile(const std::filesystem::path& manifestPath,
                        const std::filesystem::path& outputDirectory,
                        const std::string& rootNamespace);

} // namespace nc::convert
