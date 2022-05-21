#include "AssetHelpers.h"

namespace nc
{
    auto HasValidAssetExtension(const std::string& path) -> bool
    {
        const std::size_t periodPosition = path.rfind('.');
        const std::string fileExtension = path.substr(periodPosition+1);
        return fileExtension == "nca" ? true : false;
    }
}