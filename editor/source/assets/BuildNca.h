#pragma once

#include "AssetType.h"

#include <filesystem>

namespace nc::editor
{
    /** Run the asset builder on the file specified by path. On success, the
     *  nca file will be created in the same directory as the source file.
     *  Only asset type valid with the builder should be passed. */
    bool BuildNcaFile(const std::filesystem::path& path, AssetType type);
}