#pragma once

#include <filesystem>
#include <string>

namespace nc::config
{
/** @brief General project options. */
struct ProjectSettings
{
    std::string projectName = "Project Name";
    std::string logFilePath = "Diagnostics.log";
};

/** @brief Options for configuring NcAsset. */
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

/** @brief Options for component pool sizes. */
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

/** @brief Options for configuring NcGraphics. */
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

/** @brief Options for configuring NcPhysics. */
struct PhysicsSettings
{
    bool enabled = true;
    float fixedUpdateInterval = 0.01667f;
    float worldspaceExtent = 1000.0f;
};

/** @brief Options for configuring NcAudio. */
struct AudioSettings
{
    bool enabled = true;
};

/** @brief A collection of all configuration options.
 * 
 *  NcEngine must be initialized with a valid Config object. All options are
 *  immutable for the lifetime of the NcEngine instance.
*/
struct Config
{
    ProjectSettings projectSettings;
    AssetSettings assetSettings;
    MemorySettings memorySettings;
    GraphicsSettings graphicsSettings;
    PhysicsSettings physicsSettings;
    AudioSettings audioSettings;
};

/** @brief Get the ProjectSettings NcEngine was initialized with. */
auto GetProjectSettings() -> const ProjectSettings&;

/** @brief Get the AssetSettings NcEngine was initialized with. */
auto GetAssetSettings() -> const AssetSettings&;

/** @brief Get the MemorySettings NcEngine was initialized with. */
auto GetMemorySettings() -> const MemorySettings&;

/** @brief Get the GraphicsSettings NcEngine was initialized with. */
auto GetGraphicsSettings() -> const GraphicsSettings&;

/** @brief Get the PhysicsSettings NcEngine was initialized with. */
auto GetPhysicsSettings() -> const PhysicsSettings&;

/** @brief Get the AudioSettings NcEngine was initialized with. */
auto GetAudioSettings() -> const AudioSettings&;

/**
 * @brief Load a Config object from a file.
 * @param path The path to a config file.
 * @return A Config object initialized with the key=value pairs from the file.
 *         Any values absent in the file are initialized to their default
 *         values.
 * @throw NcError if there is a failure opening or reading values from the file
 *        or if Validate() fails on the resulting Config object.
 */
auto Load(const std::filesystem::path& path) -> Config;

/**
 * @brief Write a Config object's contents to a file in key=value pairs.
 * @param path The file path to be written to.
 * @param config The object data to be written.
 * @throw NcError if the file cannot be opened or created.
 */
void Save(const std::filesystem::path& path, const Config& config);

/**
 * @brief Check if a Config object is in a valid state for initializing NcEngine.
 * @param config The object to verify.
 * @return A bool indicating whether or not the object is valid.
 * @note This should be called on any manually constructed Config object prior to
 *       initializing the engine with it. It does not need to be called on an object
 *       returned from nc::config::Load().
 */
auto Validate(const Config& config) -> bool;
} // namespace nc::config
