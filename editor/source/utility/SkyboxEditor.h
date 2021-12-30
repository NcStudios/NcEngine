#pragma once

#include <filesystem>
#include <string>

namespace nc::editor
{
    std::string GetFileExtensionFromNca(std::filesystem::path ncaPath);
}