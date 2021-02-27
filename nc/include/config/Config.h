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
            unsigned maxColliders;
            unsigned maxNetworkDispatchers;
            unsigned maxRenderers;
            unsigned maxTransforms;
        } memory;

        struct
        {
            bool useNativeResolution;
            bool launchInFullscreen;
            unsigned screenWidth;
            unsigned screenHeight;
            unsigned targetFPS;
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