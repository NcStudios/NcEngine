#pragma once

#include <string>

namespace nc::config
{
    struct Config
    {
        struct
        {
            std::string projectName;
            std::string logFilePath;
        } project;
        
        struct
        {
            bool useNativeResolution;
            bool launchInFullscreen;
            unsigned int screenWidth;
            unsigned int screenHeight;
            unsigned int targetFPS;
            double nearClip;
            double farClip;
            double frameUpdateInterval;
            std::string d3dShadersPath;
            std::string vulkanShadersPath;
        } graphics;

        struct
        {
            double fixedUpdateInterval;
        } physics;
    };

    [[nodiscard]] const Config& Get();
}