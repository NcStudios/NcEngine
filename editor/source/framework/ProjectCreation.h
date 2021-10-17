#pragma once

#include <filesystem>

namespace nc::editor
{
    bool IsValidProjectParentDirectory(const std::filesystem::path& parentDirectory);
    void CreateProjectDirectories(const std::filesystem::path& projectDirectory);
    void CreateProjectFile(const std::filesystem::path& projectFilePath);
}