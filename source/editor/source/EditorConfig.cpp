#include "EditorConfig.h"
#include "config/ConfigReader.h"

#include <iostream>
#include <fstream>

namespace
{
    const auto KeyValueSeparator = '=';
    const auto RecentProjectDirectoryTag = "recent_project_directory";
    const auto RecentProjectFilePathTag = "recent_project_file_path";

    void MapConfigKeyValue(const std::string& key, const std::string& value, nc::editor::EditorConfig* out)
    {
        if(key == RecentProjectDirectoryTag)
            out->recentProjectDirectory = value;
        else if(key == RecentProjectFilePathTag)
            out->recentProjectFilePath = value;
        else
            throw std::runtime_error("MapManifestKeyValue - Unknown key: " + key);
    }
}

namespace nc::editor
{
    auto ReadConfig(const std::filesystem::path& filePath) -> EditorConfig
    {
        EditorConfig out;

        try
        {
            config::internal::Read(filePath.string(), MapConfigKeyValue, &out);
        }
        catch(const std::runtime_error& e)
        {
            std::cerr << "ReadConfig - Error reading config: " << e.what() << '\n';
            out = EditorConfig{};
        }
        
        return out;
    }

    void WriteConfig(const std::filesystem::path& filePath, const EditorConfig& config)
    {
        std::ofstream file{filePath};
        if(!file.is_open())
        {
            std::cerr << "WriteConfig - Failure opening file: " << filePath.string() << '\n';
            return;
        }

        file << RecentProjectDirectoryTag << KeyValueSeparator << config.recentProjectDirectory.string() << '\n'
             << RecentProjectFilePathTag  << KeyValueSeparator << config.recentProjectFilePath.string();
    }
}