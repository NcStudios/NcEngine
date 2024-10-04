#pragma once

#include "ncasset/AssetType.h"

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace nc::convert
{
struct Target;
void ReadManifest(const std::filesystem::path& manifestPath, std::unordered_map<asset::AssetType, std::vector<Target>>& targets);
}