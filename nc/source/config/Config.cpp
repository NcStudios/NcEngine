#include "Config.h"
#include "ConfigReader.h"
#include "DebugUtils.h"

#include <fstream>
#include <limits>
#include <type_traits>

namespace
{
    const auto CONFIG_PATH = std::string{"project/config/config.ini"};
    const auto PROJECT_NAME_KEY = std::string{"project_name"};
    const auto LOG_FILE_PATH_KEY = std::string{"log_file_path"};
    const auto USER_NAME_KEY = std::string{"user_name"};
    const auto FIXED_UPDATE_INTERVAL_KEY = std::string{"fixed_update_interval"};
    const auto USE_NATIVE_RESOLUTION_KEY = std::string{"use_native_resolution"};
    const auto LAUNCH_IN_FULLSCREEN_KEY = std::string{"launch_fullscreen"};
    const auto SCREEN_WIDTH_KEY = std::string{"screen_width"};
    const auto SCREEN_HEIGHT_KEY = std::string{"screen_height"};
    const auto TARGET_FPS_KEY = std::string{"target_fps"};
    const auto NEAR_CLIP_KEY = std::string{"near_clip"};
    const auto FAR_CLIP_KEY = std::string{"far_clip"};
    const auto SHADERS_PATH_KEY = std::string{"shaders_path"};

    void MapKeyValue(const std::string& key, const std::string& value, nc::config::Config& out)
    {
        if (key == PROJECT_NAME_KEY)
        {
            out.project.projectName = value;
        }
        else if (key == LOG_FILE_PATH_KEY)
        {
            out.project.logFilePath = value;
        }
        else if (key == USER_NAME_KEY)
        {
            out.user.userName = value;
        }
        else if (key == FIXED_UPDATE_INTERVAL_KEY)
        {
            out.physics.fixedUpdateInterval = std::stod(value);
        }
        else if (key == USE_NATIVE_RESOLUTION_KEY)
        {
            out.graphics.useNativeResolution = std::stoi(value);
        }
        else if (key == LAUNCH_IN_FULLSCREEN_KEY)
        {
            out.graphics.launchInFullscreen = std::stoi(value);
        }
        else if (key == SCREEN_WIDTH_KEY)
        {
            out.graphics.screenWidth = std::stoi(value);
        }
        else if (key == SCREEN_HEIGHT_KEY)
        {
            out.graphics.screenHeight = std::stoi(value);
        }
        else if (key == TARGET_FPS_KEY)
        {
            out.graphics.targetFPS = std::stoi(value);
            out.graphics.frameUpdateInterval = 1.0 / static_cast<double>(out.graphics.targetFPS);
        }
        else if (key == NEAR_CLIP_KEY)
        {
            out.graphics.nearClip = std::stod(value);
        }
        else if (key == FAR_CLIP_KEY)
        {
            out.graphics.farClip = std::stod(value);
        }
        else if (key == SHADERS_PATH_KEY)
        {
            out.graphics.shadersPath = value;
        }
        else
        {
            throw std::runtime_error("ConfigReader::map - unknown key");
        }
    };
} //end anonymous namespace

namespace nc::config
{
Config::Config()
{
    nc::config::Read(CONFIG_PATH, MapKeyValue, *this);
    
    if(!Validate())
    {
        throw std::runtime_error("Failed to validate config");
    }
}

void Config::Save()
{
    if (!Validate())
    {
        throw std::runtime_error("Saving config - failed to validate config");
    }

    std::ofstream outFile;
    outFile.open(CONFIG_PATH);
    if(!outFile.is_open())
    {
        throw std::runtime_error("Saving config - failed to open file");
    }

    outFile << "[project]\n"
            << PROJECT_NAME_KEY << INI_KEY_VALUE_DELIM << project.projectName << '\n'
            << LOG_FILE_PATH_KEY << INI_KEY_VALUE_DELIM << project.logFilePath << '\n'
            << "[user]\n"
            << USER_NAME_KEY << INI_KEY_VALUE_DELIM << user.userName << '\n'
            << "[physics]\n"
            << FIXED_UPDATE_INTERVAL_KEY << INI_KEY_VALUE_DELIM << physics.fixedUpdateInterval << '\n'
            << "[graphics]\n"
            << USE_NATIVE_RESOLUTION_KEY << INI_KEY_VALUE_DELIM << graphics.useNativeResolution << '\n'
            << LAUNCH_IN_FULLSCREEN_KEY << INI_KEY_VALUE_DELIM << graphics.launchInFullscreen << '\n'
            << SCREEN_WIDTH_KEY << INI_KEY_VALUE_DELIM << graphics.screenWidth << '\n'
            << SCREEN_HEIGHT_KEY << INI_KEY_VALUE_DELIM << graphics.screenHeight << '\n'
            << TARGET_FPS_KEY << INI_KEY_VALUE_DELIM << graphics.targetFPS << '\n'
            << NEAR_CLIP_KEY << INI_KEY_VALUE_DELIM << graphics.nearClip << '\n'
            << FAR_CLIP_KEY << INI_KEY_VALUE_DELIM << graphics.farClip << '\n'
            << SHADERS_PATH_KEY << INI_KEY_VALUE_DELIM << graphics.shadersPath;

    outFile.close();
}

bool Config::Validate()
{
    return { (user.userName != "") &&
             (project.projectName != "") &&
             (project.logFilePath != "") &&
             (physics.fixedUpdateInterval > 0.0f) &&
             (graphics.screenWidth != 0) &&
             (graphics.screenHeight != 0) &&
             (graphics.targetFPS != 0) &&
             (graphics.frameUpdateInterval > 0.0f) &&
             (graphics.nearClip > 0.0f) &&
             (graphics.farClip > 0.0f) &&
             (graphics.shadersPath != "")};
}
} // end namespace nc::config 