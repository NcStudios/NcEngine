#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace nc::editor
{
struct ProjectData
{
    std::string name;
    std::filesystem::path projectDirectory;
    std::filesystem::path projectFile;
    std::vector<std::string> scenes;
    bool open = false;
};
} // namespace nc::editor
