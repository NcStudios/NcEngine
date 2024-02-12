/**
 * @file Config.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

#include <filesystem>
#include <string>

namespace nc::config
{
/** @brief General project options. */
struct ProjectSettings
{
    std::string projectName = "Project Name";
    std::string logFilePath = "NcEngine.log";
    size_t logMaxFileSize = 1000000ull;
};

/**
 * @brief Options for configuring NcAsset.
 * 
 * Each option specifies the search directory for an asset type. Paths may be
 * absolute or relative to the calling executable.
 */
struct AssetSettings
{
    /** @todo These defaults don't make much sense since we don't know the install path. */
    std::string audioClipsPath = "resources/assets/audio_clips/";
    std::string concaveCollidersPath = "resources/assets/concave_colliders/";
    std::string hullCollidersPath = "resources/assets/hull_colliders/";
    std::string meshesPath = "resources/assets/meshes/";
    std::string shadersPath = "resources/assets/shaders/";
    std::string skeletalAnimationsPath = "resources/assets/skeletal_animations/";
    std::string texturesPath = "resources/assets/textures/";
    std::string cubeMapsPath = "resources/assets/cube_maps";
};

/**
 * @brief Options for configuring pre-allocated pools.
 * 
 * Each option specifies the maximum number of instances of a particular type
 * that can exist at one time.
 */
struct MemorySettings
{
    unsigned maxDynamicColliders = 25000;
    unsigned maxStaticColliders = 25000;
    unsigned maxNetworkDispatchers = 0;
    unsigned maxParticleEmitters = 1000;
    unsigned maxRenderers = 100000;
    unsigned maxTransforms = 100000;
    unsigned maxPointLights = 10;
    unsigned maxSkeletalAnimations = 1000;
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
    unsigned bufferFrames = 512u; // must be a multiple of two in the range [16, 2056]
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

/**
 * @brief Load a Config object from a file.
 * @param path The path to a config file.
 * @return A Config object initialized with key-value pairs from the file.
 * @throw NcError if there is a failure opening or reading values from the file
 *        or if Validate() fails on the resulting Config object.
 * 
 * The input file may contain any config options as equals-separated key-value
 * pairs, each on their own line. Any options absent in the file will be
 * initialized to their default values.
 * 
 * The following are ignored:
 *   - Unrecognized options
 *   - Leading/trailing whitespace and empty lines
 *   - Line comments starting with ';' or '#'
 *   - Sections (e.g. [my_section])
 * 
 * For a listing of all keys see the Sample or create a default file with:
 *   Save("config.ini", nc::config::Config{});
 */
auto Load(const std::filesystem::path& path) -> Config;

/**
 * @brief Write a Config object's contents to a file.
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
} // namespace nc::config
