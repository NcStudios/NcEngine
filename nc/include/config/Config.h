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
            unsigned maxDynamicColliders;
            unsigned maxStaticColliders;
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
            float nearClip;
            float farClip;
            float frameUpdateInterval;
            std::string d3dShadersPath;
            std::string vulkanShadersPath;
        } graphics;

        struct
        {
            float fixedUpdateInterval;
            unsigned octreeDensityThreshold;
            float octreeMinimumExtent;
        } physics;
    };

    [[nodiscard]] const Config& Get();
}