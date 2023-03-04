#include "config/Config.h"
#include "config/ConfigReader.h"
#include "ConfigInternal.h"

#include <fstream>
#include <limits>
#include <memory>
#include <type_traits>

namespace
{
    auto g_config = nc::config::Config{};

    using namespace std::literals;

    // project
    constexpr auto ProjectNameKey = "project_name"sv;
    constexpr auto LogFilePathKey = "log_file_path"sv;

    // asset
    constexpr auto AudioClipsPathKey = "audio_clips_path"sv;
    constexpr auto ConcaveCollidersKey = "concave_colliders_path"sv;
    constexpr auto HullCollidersKey = "hull_colliders_path"sv;
    constexpr auto MeshesPathKey = "meshes_path"sv;
    constexpr auto ShadersPathKey = "shaders_path"sv;
    constexpr auto TexturesPathKey = "textures_path"sv;
    constexpr auto CubeMapsPathKey = "cubemaps_path"sv;

    // memory
    constexpr auto MaxDynamicCollidersKey = "max_dynamic_colliders"sv;
    constexpr auto MaxStaticCollidersKey = "max_static_colliders"sv;
    constexpr auto MaxNetworkDispatchersKey = "max_network_dispatchers"sv;
    constexpr auto MaxParticleEmittersKey = "max_particle_emitters"sv;
    constexpr auto MaxRenderersKey = "max_renderers"sv;
    constexpr auto MaxTransformsKey = "max_transforms"sv;
    constexpr auto MaxPointLightsKey = "max_point_lights"sv;
    constexpr auto MaxTexturesKey = "max_textures"sv;

    // physics
    constexpr auto PhysicsEnabledKey = "physics_enabled"sv;
    constexpr auto FixedUpdateIntervalKey = "fixed_update_interval"sv;
    constexpr auto WorldspaceExtentKey = "worldspace_extent"sv;

    // graphics
    constexpr auto GraphicsEnabledKey = "graphics_enabled"sv;
    constexpr auto UseNativeResolutionKey = "use_native_resolution"sv;
    constexpr auto LaunchInFullscreenKey = "launch_fullscreen"sv;
    constexpr auto ScreenWidthKey = "screen_width"sv;
    constexpr auto ScreenHeightKey = "screen_height"sv;
    constexpr auto TargetFpsKey = "target_fps"sv;
    constexpr auto NearClipKey = "near_clip"sv;
    constexpr auto FarClipKey = "far_clip"sv;
    constexpr auto UseShadowsKey = "use_shadows"sv; /** @todo: Make this a property of the material */
    constexpr auto AntialiasingKey = "antialiasing"sv;
    constexpr auto UseValidationLayersKey = "use_validation_layers"sv;

    // audio
    constexpr auto AudioEnabledKey = "audio_enabled"sv;

    void MapKeyValue(const std::string& key, const std::string& value, nc::config::Config* out)
    {
        // project
        if (key == ProjectNameKey)
            out->projectSettings.projectName = value;
        else if (key == LogFilePathKey)
            out->projectSettings.logFilePath = value;

        // asset
        else if (key == AudioClipsPathKey)
            out->assetSettings.audioClipsPath = value;
        else if (key == ConcaveCollidersKey)
            out->assetSettings.concaveCollidersPath = value;
        else if (key == HullCollidersKey)
            out->assetSettings.hullCollidersPath = value;
        else if (key == MeshesPathKey)
            out->assetSettings.meshesPath = value;
        else if (key == ShadersPathKey)
            out->assetSettings.shadersPath = value;
        else if (key == TexturesPathKey)
            out->assetSettings.texturesPath = value;
        else if (key == CubeMapsPathKey)
            out->assetSettings.cubeMapsPath = value;

        // memory
        else if(key == MaxDynamicCollidersKey)
            out->memorySettings.maxDynamicColliders = std::stoi(value);
        else if(key == MaxStaticCollidersKey)
            out->memorySettings.maxStaticColliders = std::stoi(value);
        else if(key == MaxNetworkDispatchersKey)
            out->memorySettings.maxNetworkDispatchers = std::stoi(value);
        else if(key == MaxParticleEmittersKey)
            out->memorySettings.maxParticleEmitters = std::stoi(value);
        else if(key == MaxRenderersKey)
            out->memorySettings.maxRenderers = std::stoi(value);
        else if(key == MaxTransformsKey)
            out->memorySettings.maxTransforms = std::stoi(value);
        else if(key == MaxPointLightsKey)
            out->memorySettings.maxPointLights = std::stoi(value);
        else if(key == MaxTexturesKey)
            out->memorySettings.maxTextures = std::stoi(value);

        // physics
        else if (key == PhysicsEnabledKey)
            out->physicsSettings.enabled = static_cast<bool>(std::stoi(value));
        else if (key == FixedUpdateIntervalKey)
            out->physicsSettings.fixedUpdateInterval = std::stof(value);
        else if (key == WorldspaceExtentKey)
            out->physicsSettings.worldspaceExtent = std::stof(value);

        // graphics
        else if (key == GraphicsEnabledKey)
            out->graphicsSettings.enabled = static_cast<bool>(std::stoi(value));
        else if (key == UseNativeResolutionKey)
            out->graphicsSettings.useNativeResolution = std::stoi(value);
        else if (key == LaunchInFullscreenKey)
            out->graphicsSettings.launchInFullscreen = std::stoi(value);
        else if (key == ScreenWidthKey)
            out->graphicsSettings.screenWidth = std::stoi(value);
        else if (key == ScreenHeightKey)
            out->graphicsSettings.screenHeight = std::stoi(value);
        else if (key == TargetFpsKey)
            out->graphicsSettings.targetFPS = std::stoi(value);
        else if (key == NearClipKey)
            out->graphicsSettings.nearClip = std::stof(value);
        else if (key == FarClipKey)
            out->graphicsSettings.farClip = std::stof(value);
        else if (key == UseShadowsKey)
            out->graphicsSettings.useShadows = std::stoi(value);
        else if (key == AntialiasingKey)
            out->graphicsSettings.antialiasing = std::stoi(value);
        else if (key == UseValidationLayersKey)
            out->graphicsSettings.useValidationLayers = std::stoi(value);

        // audio
        else if (key == AudioEnabledKey)
            out->audioSettings.enabled = static_cast<bool>(std::stoi(value));
        else
            throw nc::NcError(std::string{"Unknown config key: "} + key);
    };
} //end anonymous namespace

namespace nc::config
{
    /* Api function implementation */
    const ProjectSettings& GetProjectSettings()
    {
        return g_config.projectSettings;
    }

    const AssetSettings& GetAssetSettings()
    {
        return g_config.assetSettings;
    }

    const MemorySettings& GetMemorySettings()
    {
        return g_config.memorySettings;
    }

    const GraphicsSettings& GetGraphicsSettings()
    {
        return g_config.graphicsSettings;
    }

    const PhysicsSettings& GetPhysicsSettings()
    {
        return g_config.physicsSettings;
    }

    const AudioSettings& GetAudioSettings()
    {
        return g_config.audioSettings;
    }

    auto Load(std::string_view path) -> Config
    {
        Config out;
        internal::Read(path, MapKeyValue, &out);

        if(!Validate(out))
            throw NcError("Config validation failed");

        return out;
    }

    void Save(std::string_view path, const Config& config)
    {
        std::ofstream file{path.data()};
        if(!file.is_open())
            throw NcError(std::string{"Failure opening: "} + path.data());

        file << "[project]\n"
             << ProjectNameKey           << '=' << config.projectSettings.projectName          << '\n'
             << LogFilePathKey           << '=' << config.projectSettings.logFilePath          << '\n'
             << "[asset]\n"
             << AudioClipsPathKey        << '=' << config.assetSettings.audioClipsPath       << '\n'
             << ConcaveCollidersKey      << '=' << config.assetSettings.concaveCollidersPath << '\n'
             << HullCollidersKey         << '=' << config.assetSettings.hullCollidersPath    << '\n'
             << MeshesPathKey            << '=' << config.assetSettings.meshesPath           << '\n'
             << ShadersPathKey           << '=' << config.assetSettings.shadersPath          << '\n'
             << TexturesPathKey          << '=' << config.assetSettings.texturesPath         << '\n'
             << CubeMapsPathKey          << '=' << config.assetSettings.cubeMapsPath         << '\n'
             << "[memory]\n"
             << MaxDynamicCollidersKey   << '=' << config.memorySettings.maxDynamicColliders   << '\n'
             << MaxStaticCollidersKey    << '=' << config.memorySettings.maxStaticColliders    << '\n'
             << MaxNetworkDispatchersKey << '=' << config.memorySettings.maxNetworkDispatchers << '\n'
             << MaxParticleEmittersKey   << '=' << config.memorySettings.maxParticleEmitters   << '\n'
             << MaxRenderersKey          << '=' << config.memorySettings.maxRenderers          << '\n'
             << MaxTransformsKey         << '=' << config.memorySettings.maxTransforms         << '\n'
             << MaxTexturesKey           << '=' << config.memorySettings.maxTextures           << '\n'
             << "[physics]\n"
             << PhysicsEnabledKey        << '=' << config.physicsSettings.enabled              << '\n'
             << FixedUpdateIntervalKey   << '=' << config.physicsSettings.fixedUpdateInterval  << '\n'
             << WorldspaceExtentKey      << '=' << config.physicsSettings.worldspaceExtent     << '\n'
             << "[graphics]\n"
             << GraphicsEnabledKey       << '=' << config.graphicsSettings.enabled             << '\n'
             << UseNativeResolutionKey   << '=' << config.graphicsSettings.useNativeResolution << '\n'
             << LaunchInFullscreenKey    << '=' << config.graphicsSettings.launchInFullscreen  << '\n'
             << ScreenWidthKey           << '=' << config.graphicsSettings.screenWidth         << '\n'
             << ScreenHeightKey          << '=' << config.graphicsSettings.screenHeight        << '\n'
             << TargetFpsKey             << '=' << config.graphicsSettings.targetFPS           << '\n'
             << NearClipKey              << '=' << config.graphicsSettings.nearClip            << '\n'
             << FarClipKey               << '=' << config.graphicsSettings.farClip             << '\n'
             << UseShadowsKey            << '=' << config.graphicsSettings.useShadows          << '\n'
             << AntialiasingKey          << '=' << config.graphicsSettings.antialiasing        << '\n'
             << UseValidationLayersKey   << '=' << config.graphicsSettings.useValidationLayers << '\n'
             << "[audio]\n"
             << AudioEnabledKey          << '=' << config.audioSettings.enabled                << '\n';
    }

    bool Validate(const Config& config)
    {
        return { (config.projectSettings.projectName != "") &&
                 (config.projectSettings.logFilePath != "") &&
                 (config.assetSettings.audioClipsPath != "") &&
                 (config.assetSettings.concaveCollidersPath != "") &&
                 (config.assetSettings.hullCollidersPath != "") &&
                 (config.assetSettings.meshesPath != "") &&
                 (config.assetSettings.shadersPath != "") &&
                 (config.assetSettings.texturesPath != "") &&
                 (config.assetSettings.cubeMapsPath != "") &&
                 (config.physicsSettings.fixedUpdateInterval > 0.0f) &&
                 (config.physicsSettings.worldspaceExtent > 0.0f) &&
                 (config.graphicsSettings.screenWidth != 0) &&
                 (config.graphicsSettings.screenHeight != 0) &&
                 (config.graphicsSettings.targetFPS != 0) &&
                 (config.graphicsSettings.nearClip > 0.0f) &&
                 (config.graphicsSettings.farClip > 0.0f) &&
                 (config.graphicsSettings.antialiasing > 0)};
    }

    /* Internal function implementation */
    void SetConfig(const Config& config)
    {
        g_config = config;
    }
} // end namespace nc::config