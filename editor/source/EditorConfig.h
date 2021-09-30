#pragma once

#include <filesystem>
#include <string>

namespace nc::editor
{
    struct EditorConfig
    {
        std::string projectName;
        std::filesystem::path recentProjectDirectory;
    };

    auto ReadConfig(const std::filesystem::path& filePath) -> EditorConfig;
    void WriteConfig(const std::filesystem::path& filePath, const EditorConfig& config);
}