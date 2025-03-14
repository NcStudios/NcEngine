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

auto AssetNameToNcaPath(const std::filesystem::path& assetName, const std::filesystem::path& outDir) -> std::filesystem::path
{
    return (outDir / assetName).replace_extension(".nca").make_preferred();
}
}
