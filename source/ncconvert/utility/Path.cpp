#include "Path.h"

#include <algorithm>

namespace nc::convert
{
auto ValidateInputFileExtension(const std::filesystem::path& path, std::span<const std::string> validExtensions) -> bool
{
    if (!std::filesystem::is_regular_file(path))
    {
        return false;
    }

    const auto extension = path.extension().string();
    return std::ranges::find(validExtensions, extension) != validExtensions.end();
}

auto AssetNameToNcaPath(std::string assetName, const std::filesystem::path& outDir) -> std::filesystem::path
{
    const auto ncaFileName = std::filesystem::path{assetName}.replace_extension(".nca");
    return (outDir / ncaFileName).make_preferred();
}
}
