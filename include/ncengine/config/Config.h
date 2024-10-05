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

/** @brief Settings for configuring the engine run loop and executor. */
struct EngineSettings
{
    float timeStep = 0.01667f;    // Set to 0 for variable time step
    float maxTimeStep = 0.1f;     // Clamp delta time below this value
    unsigned threadCount = 8u;    // Set to 0 to use std::hardware_concurrency
    bool buildTasksOnInit = true; // Build tasks automatically on engine initialization or require explicit building
};

/**
 * @brief Options for configuring NcAsset.
 * 
 * Each option specifies the search directory for an asset type. Paths may be
 * absolute or relative to the calling executable.
 */
struct AssetSettings
{
    std::string audioClipsPath = "assets/audio_clips/";
    std::string concaveCollidersPath = "assets/concave_colliders/";
    std::string hullCollidersPath = "assets/hull_colliders/";
    std::string meshesPath = "assets/meshes/";
    std::string shadersPath = "assets/shaders/";
    std::string skeletalAnimationsPath = "assets/skeletal_animations/";
    std::string texturesPath = "assets/textures/";
    std::string cubeMapsPath = "assets/cube_maps";
    std::string fontsPath = "assets/fonts";
};

/**
 * @brief Options for configuring pre-allocated pools.
 * 
 * Each option specifies the maximum number of instances of a particular type
 * that can exist at one time.
 */
struct MemorySettings
{
    unsigned maxRigidBodies = 15000;
    unsigned maxNetworkDispatchers = 0;
    unsigned maxParticleEmitters = 1000;
    unsigned maxRenderers = 100000;
    unsigned maxTransforms = 100000;
    unsigned maxPointLights = 10;
    unsigned maxSpotLights = 10;
    unsigned maxSkeletalAnimations = 1000;
    unsigned maxTextures = 1000;
    unsigned maxCubeMaps = 10;
    unsigned maxParticles = 100000;
};

/** @brief Options for configuring NcGraphics. */
struct GraphicsSettings
{
    bool enabled = true;              ///< enable the NcGraphics module
    /**
    api is the target graphics api from a predetermined list (narrowed at build time by platform)
    of vulkan, d3d12, d3d11, opengl. See nc::graphics::GetSupportedApis().
    If the target api is not detected as compatible, the next in the list returned by GetSupportedApis()
    will be chosen as a fallback.
     */
    std::string api = "vulkan";       ///< possible values: vulkan, d3d12, d3d11, opengl
    bool isHeadless = false;          ///< run the api in headless mode
    bool useNativeResolution = false; ///< use the monitor's native resolution
    bool launchInFullscreen = false;  ///< launch a fullscreen window
    unsigned screenWidth = 1000;      ///< width of the screen
    unsigned screenHeight = 1000;     ///< height of the screen
    unsigned targetFPS = 60;          ///< target frame rate
    float nearClip = 0.5f;            ///< the near z clip of the camera frustum
    float farClip = 400.0f;           ///< the far z clip of the camera frustum
    bool useShadows = true;           ///< enable shadow mapping and shadow rendering
    unsigned antialiasing = 8u;       ///< the number of samples for MSAA
    bool useValidationLayers = false; ///< turn on validation layers in debug builds
};

/** @brief Options for configuring NcPhysics. */
struct PhysicsSettings
{
    bool enabled = true;                            ///< enable the NcPhysics module
    unsigned tempAllocatorSize = 64 * 1024 * 1024;  ///< size of per-frame allocaor; needs to be large enough to account for maximums below (bytes)
    unsigned maxBodyPairs = 50000;                  ///< max number of simultaneous body interactions
    unsigned maxContacts = 30000;                   ///< max number of simultaneous contacts
    unsigned velocitySteps = 10;                    ///< number of velocity solver iterations to use (>=2)
    unsigned positionSteps = 2;                     ///< number of position solver iterations to use
    float baumgarteStabilization = 0.2f;            ///< factor for position error correction ([0, 1])
    float speculativeContactDistance = 0.02f;       ///< object radius for contact detection (meters)
    float penetrationSlop = 0.02f;                  ///< distance objects are allowed to overlap (meters)
    float timeBeforeSleep = 0.5f;                   ///< time until objects are allowed to sleep (seconds)
    float sleepThreshold = 0.03f;                   ///< velocity below which objects are put to sleep (m/s)
};

/** @brief Options for configuring NcAudio. */
struct AudioSettings
{
    bool enabled = true;
    unsigned bufferFrames = 512u; // must be a power of two in the range [16, 2048]
};

/** @brief A collection of all configuration options.
 * 
 *  NcEngine must be initialized with a valid Config object. All options are
 *  immutable for the lifetime of the NcEngine instance.
*/
struct Config
{
    ProjectSettings projectSettings;
    EngineSettings engineSettings;
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

/** @brief Get the EngineSettings NcEngine was initialized with. */
auto GetEngineSettings() -> const EngineSettings&;

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
