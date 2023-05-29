
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
    /** @todo These defaults don't make much sense since we don't know the install path. */
    std::string audioClipsPath = "resources/assets/audio_clips/";
    std::string concaveCollidersPath = "resources/assets/concave_colliders/";
    std::string hullCollidersPath = "resources/assets/hull_colliders/";
    std::string meshesPath = "resources/assets/meshes/";
    std::string shadersPath = "resources/assets/shaders/";
    std::string texturesPath = "resources/assets/textures/";
    std::string cubeMapsPath = "resources/assets/cube_maps";
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
    bool enabled = true;
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
    bool enabled = true;
    float fixedUpdateInterval = 0.01667f;
    float worldspaceExtent = 1000.0f;
};

struct AudioSettings
{
    bool enabled = true;
};

struct Config
{
    ProjectSettings projectSettings;
    AssetSettings assetSettings;
    MemorySettings memorySettings;
    GraphicsSettings graphicsSettings;
    PhysicsSettings physicsSettings;
    AudioSettings audioSettings;
};

[[nodiscard]] auto GetProjectSettings() -> const ProjectSettings&;
[[nodiscard]] auto GetAssetSettings() -> const AssetSettings&;
[[nodiscard]] auto GetMemorySettings() -> const MemorySettings&;
[[nodiscard]] auto GetGraphicsSettings() -> const GraphicsSettings&;
[[nodiscard]] auto GetPhysicsSettings() -> const PhysicsSettings&;
[[nodiscard]] auto GetAudioSettings() -> const AudioSettings&;
[[nodiscard]] auto Load(std::string_view iniPath) -> Config;
[[nodiscard]] bool Validate(const Config& config);
void Save(std::string_view path, const Config& config);
} // namespace nc::config