#include "SkyboxEditor.h"
#include "utility/Output.h"

#include <fstream>

namespace nc::editor
{
    std::string GetFileExtensionFromNca(std::filesystem::path ncaPath)
    {
        auto file = std::ifstream(ncaPath);

        if(!file.is_open())
        {
            Output::LogError("Failure opening nca file:", ncaPath.string());
            return "";
        }

        std::string line;
        while(!file.eof())
        {
            if(file.fail())
            {
                Output::LogError("Failure reading nca file:", ncaPath.string());
                file.close();
                return "";
            }

            std::getline(file, line, '\n');

            if(line.starts_with("back"))
            {
                file.close();
                return std::filesystem::path(line).extension().string();
            }
        }

        return "";
    }
}