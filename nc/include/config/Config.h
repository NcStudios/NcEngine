
#pragma once

#include <string>

namespace nc::config
{
    struct ProjectSettings
    {
        std::string projectName;
        std::string logFilePath;
    };

    struct MemorySettings
    {
        unsigned maxDynamicColliders;
        unsigned maxStaticColliders;
        unsigned maxNetworkDispatchers;
        unsigned maxParticleEmitters;
        unsigned maxRenderers;
        unsigned maxTransforms;
        unsigned maxPointLights;
        unsigned maxTextures;
    };

    struct GraphicsSettings
    {
        bool useNativeResolution;
        bool launchInFullscreen;
        unsigned screenWidth;
        unsigned screenHeight;
        unsigned targetFPS;
        float nearClip;
        float farClip;
        float frameUpdateInterval;
        std::string shadersPath;
        bool useShadows;
    };

    struct PhysicsSettings
    {
        float fixedUpdateInterval;
        float worldspaceExtent;
        unsigned octreeDensityThreshold;
        float octreeMinimumExtent;
    };

    [[nodiscard]] auto GetProjectSettings() -> const ProjectSettings&;
    [[nodiscard]] auto GetMemorySettings() -> const MemorySettings&;
    [[nodiscard]] auto GetGraphicsSettings() -> const GraphicsSettings&;
    [[nodiscard]] auto GetPhysicsSettings() -> const PhysicsSettings&;
}