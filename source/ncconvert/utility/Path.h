#pragma once

#include <filesystem>
#include <span>
#include <string>

namespace nc::convert
{
auto ValidateInputFileExtension(const std::filesystem::path& path, std::span<const std::string> validExtensions) -> bool;
auto AssetNameToNcaPath(std::string assetName, const std::filesystem::path& outDir) -> std::filesystem::path;
}
