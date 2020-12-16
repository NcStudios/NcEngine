#include "ProjectConfig.h"
#include "nc/source/config/ConfigReader.h"

#include <fstream>

namespace
{
    const auto CONFIG_PATH = std::string{"project/config/config.ini"};
    const auto USER_NAME_KEY = std::string{"user_name"};

    void MapKeyValue(std::string key, std::string value, project::config::ProjectConfig& out)
    {
        if(key == USER_NAME_KEY)
        {
            out.userName = value;
        }
        else
        {
            throw std::runtime_error("Invalid key in project config");
        }
    }
}

namespace project::config
{
    void ProjectConfig::Load()
    {
        nc::config::Read(CONFIG_PATH, MapKeyValue, *this);
    }

    void ProjectConfig::Save()
    {
        std::ofstream outFile;
        outFile.open(CONFIG_PATH);
        outFile << USER_NAME_KEY << nc::config::INI_KEY_VALUE_DELIM << userName;
        outFile.close();
    }
}