#include "AssetUtilities.h"

namespace nc::asset
{
auto HasValidAssetExtension(const std::string& path) -> bool
{
    const auto periodPosition = path.rfind('.');
    if (periodPosition == std::string::npos)
    {
        return false;
    }

    const auto fileExtension = path.substr(periodPosition+1);
    return fileExtension == "nca" ? true : false;
}
} // namespace nc::asset
