
#pragma once

#include <string>

namespace nc::config
{
    struct ProjectSettings
    {
        std::string projectName;
        std::string logFilePath;
        std::string audioClipsPath;
        std::string concaveCollidersPath;
        std::string hullCollidersPath;
        std::string meshesPath;
        std::string shadersPath;
        std::string texturesPath;
        std::string cubeMapsPath;
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
        bool useShadows;
    };

    struct PhysicsSettings
    {
        float fixedUpdateInterval;
        float worldspaceExtent;
    };

    [[nodiscard]] auto GetProjectSettings() -> const ProjectSettings&;
    [[nodiscard]] auto GetMemorySettings() -> const MemorySettings&;
    [[nodiscard]] auto GetGraphicsSettings() -> const GraphicsSettings&;
    [[nodiscard]] auto GetPhysicsSettings() -> const PhysicsSettings&;

    struct Config
    {
        nc::config::ProjectSettings projectSettings;
        nc::config::MemorySettings memorySettings;
        nc::config::GraphicsSettings graphicsSettings;
        nc::config::PhysicsSettings physicsSettings;
    };

    auto Load(const std::string& path) -> Config;
    void Save(const std::string& path, const Config& config);
    bool Validate(const Config& config);
}