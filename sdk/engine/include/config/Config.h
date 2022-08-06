
#pragma once

#include <string>

namespace nc::config
{
struct ProjectSettings
{
    std::string projectName = "Project Name";
    std::string logFilePath = "Diagnostics.log";
};

struct AssetSettings
{
    /** @todo These path defaults are no good. */
    std::string audioClipsPath = "nc/resources/audio_clips/";
    std::string concaveCollidersPath = "nc/resources/concave_colliders/";
    std::string hullCollidersPath = "nc/resources/hull_colliders/";
    std::string meshesPath = "nc/resources/meshes/";
    std::string shadersPath = "nc/resources/shaders/";
    std::string texturesPath = "nc/resources/textures/";
    std::string cubeMapsPath = "";
};

struct MemorySettings
{
    unsigned maxDynamicColliders = 25000;
    unsigned maxStaticColliders = 25000;
    unsigned maxNetworkDispatchers = 0;
    unsigned maxParticleEmitters = 1000;
    unsigned maxRenderers = 100000;
    unsigned maxTransforms = 100000;
    unsigned maxPointLights = 10;
    unsigned maxTextures = 1000;
};

struct GraphicsSettings
{
    bool useNativeResolution = false;
    bool launchInFullscreen = false;
    unsigned screenWidth = 1000;
    unsigned screenHeight = 1000;
    unsigned targetFPS = 60;
    float nearClip = 0.5f;
    float farClip = 400.0f;
    bool useShadows = true;
    unsigned antialiasing = 8u;
    bool useValidationLayers = false;
};

struct PhysicsSettings
{
    float fixedUpdateInterval = 0.01667f;
    float worldspaceExtent = 1000.0f;
};

struct Config
{
    nc::config::ProjectSettings projectSettings;
    nc::config::AssetSettings assetSettings;
    nc::config::MemorySettings memorySettings;
    nc::config::GraphicsSettings graphicsSettings;
    nc::config::PhysicsSettings physicsSettings;
};

[[nodiscard]] auto GetProjectSettings() -> const ProjectSettings&;
[[nodiscard]] auto GetAssetSettings() -> const AssetSettings&;
[[nodiscard]] auto GetMemorySettings() -> const MemorySettings&;
[[nodiscard]] auto GetGraphicsSettings() -> const GraphicsSettings&;
[[nodiscard]] auto GetPhysicsSettings() -> const PhysicsSettings&;
[[nodiscard]] auto Load(std::string_view iniPath) -> Config;
[[nodiscard]] bool Validate(const Config& config);
void Save(std::string_view path, const Config& config);
} // namespace nc::config