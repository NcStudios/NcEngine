#include "config/Config.h"
#include "config/ConfigReader.h"
#include "ConfigInternal.h"
#include "debug/Utils.h"

#include <fstream>
#include <limits>
#include <memory>
#include <type_traits>

namespace
{
    std::unique_ptr<nc::config::Config> g_config = nullptr;
    std::string g_configPath = "";

    // project
    const auto ProjectNameKey = std::string{"project_name"};
    const auto LogFilePathKey = std::string{"log_file_path"};
    const auto AudioClipsPathKey = std::string{"audio_clips_path"};
    const auto ConcaveCollidersKey = std::string{"concave_colliders_path"};
    const auto HullCollidersKey = std::string{"hull_colliders_path"};
    const auto MeshesPathKey = std::string{"meshes_path"};
    const auto ShadersPathKey = std::string{"shaders_path"};
    const auto TexturesPathKey = std::string{"textures_path"};
    const auto CubeMapsPathKey = std::string{"cubemaps_path"};

    // memory
    const auto MaxDynamicCollidersKey = std::string{"max_dynamic_colliders"};
    const auto MaxStaticCollidersKey = std::string{"max_static_colliders"};
    const auto MaxNetworkDispatchersKey = std::string{"max_network_dispatchers"};
    const auto MaxParticleEmittersKey = std::string{"max_particle_emitters"};
    const auto MaxRenderersKey = std::string{"max_renderers"};
    const auto MaxTransformsKey = std::string{"max_transforms"};
    const auto MaxPointLightsKey = std::string{"max_point_lights"};
    const auto MaxTexturesKey = std::string{"max_textures"};

    // physics
    const auto FixedUpdateIntervalKey = std::string{"fixed_update_interval"};
    const auto WorldspaceExtentKey = std::string{"worldspace_extent"};

    // graphics
    const auto UseNativeResolutionKey = std::string{"use_native_resolution"};
    const auto LaunchInFullscreenKey = std::string{"launch_fullscreen"};
    const auto ScreenWidthKey = std::string{"screen_width"};
    const auto ScreenHeightKey = std::string{"screen_height"};
    const auto TargetFpsKey = std::string{"target_fps"};
    const auto NearClipKey = std::string{"near_clip"};
    const auto FarClipKey = std::string{"far_clip"};
    const auto UseShadowsKey = std::string{"use_shadows"}; /** @todo: Make this a property of the material */

    void MapKeyValue(const std::string& key, const std::string& value, nc::config::Config* out)
    {
        // project
        if (key == ProjectNameKey)
            out->projectSettings.projectName = value;
        else if (key == LogFilePathKey)
            out->projectSettings.logFilePath = value;
        else if (key == AudioClipsPathKey)
            out->projectSettings.audioClipsPath = value;
        else if (key == ConcaveCollidersKey)
            out->projectSettings.concaveCollidersPath = value;
        else if (key == HullCollidersKey)
            out->projectSettings.hullCollidersPath = value;
        else if (key == MeshesPathKey)
            out->projectSettings.meshesPath = value;
        else if (key == ShadersPathKey)
            out->projectSettings.shadersPath = value;
        else if (key == TexturesPathKey)
            out->projectSettings.texturesPath = value;
        else if (key == CubeMapsPathKey)
            out->projectSettings.cubeMapsPath = value;

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
        else if (key == FixedUpdateIntervalKey)
            out->physicsSettings.fixedUpdateInterval = std::stof(value);
        else if (key == WorldspaceExtentKey)
            out->physicsSettings.worldspaceExtent = std::stof(value);

        // graphics
        else if (key == UseNativeResolutionKey)
            out->graphicsSettings.useNativeResolution = std::stoi(value);
        else if (key == LaunchInFullscreenKey)
            out->graphicsSettings.launchInFullscreen = std::stoi(value);
        else if (key == ScreenWidthKey)
            out->graphicsSettings.screenWidth = std::stoi(value);
        else if (key == ScreenHeightKey)
            out->graphicsSettings.screenHeight = std::stoi(value);
        else if (key == TargetFpsKey)
        {
            out->graphicsSettings.targetFPS = std::stoi(value);
            out->graphicsSettings.frameUpdateInterval = 1.0 / static_cast<float>(out->graphicsSettings.targetFPS);
        }
        else if (key == NearClipKey)
            out->graphicsSettings.nearClip = std::stof(value);
        else if (key == FarClipKey)
            out->graphicsSettings.farClip = std::stof(value);
        else if (key == UseShadowsKey)
            out->graphicsSettings.useShadows = std::stoi(value);
        else
            throw nc::NcError("Unknown config key");
    };
} //end anonymous namespace

namespace nc::config
{
    /* Api function implementation */
    const ProjectSettings& GetProjectSettings()
    {
        IF_THROW(!g_config, "config::GetProjectSettings - No config loaded");
        return g_config->projectSettings;
    }

    const MemorySettings& GetMemorySettings()
    {
        IF_THROW(!g_config, "config::GetMemorySettings - No config loaded");
        return g_config->memorySettings;
    }

    const GraphicsSettings& GetGraphicsSettings()
    {
        IF_THROW(!g_config, "config::GetGraphicsSettings - No config loaded");
        return g_config->graphicsSettings;
    }

    const PhysicsSettings& GetPhysicsSettings()
    {
        IF_THROW(!g_config, "config::GetPhysicsSettings - No config loaded");
        return g_config->physicsSettings;
    }

    auto Load(const std::string& path) -> Config
    {
        Config out;
        Read(path, MapKeyValue, &out);

        if(!Validate(out))
            throw NcError("Config validation failed");

        return out;
    }

    void Save(const std::string& path, const Config& config)
    {
        std::ofstream file{path};
        if(!file.is_open())
            throw NcError("Failure opening: " + path);

        file << "[project]\n"
             << ProjectNameKey           << '=' << config.projectSettings.projectName          << '\n'
             << LogFilePathKey           << '=' << config.projectSettings.logFilePath          << '\n'
             << AudioClipsPathKey        << '=' << config.projectSettings.audioClipsPath       << '\n'
             << ConcaveCollidersKey      << '=' << config.projectSettings.concaveCollidersPath << '\n'
             << HullCollidersKey         << '=' << config.projectSettings.hullCollidersPath    << '\n'
             << MeshesPathKey            << '=' << config.projectSettings.meshesPath           << '\n'
             << ShadersPathKey           << '=' << config.projectSettings.shadersPath          << '\n'
             << TexturesPathKey          << '=' << config.projectSettings.texturesPath         << '\n'
             << CubeMapsPathKey          << '=' << config.projectSettings.cubeMapsPath         << '\n'
             << "[memory]\n"
             << MaxDynamicCollidersKey   << '=' << config.memorySettings.maxDynamicColliders   << '\n'
             << MaxStaticCollidersKey    << '=' << config.memorySettings.maxStaticColliders    << '\n'
             << MaxNetworkDispatchersKey << '=' << config.memorySettings.maxNetworkDispatchers << '\n'
             << MaxParticleEmittersKey   << '=' << config.memorySettings.maxParticleEmitters   << '\n'
             << MaxRenderersKey          << '=' << config.memorySettings.maxRenderers          << '\n'
             << MaxTransformsKey         << '=' << config.memorySettings.maxTransforms         << '\n'
             << MaxTexturesKey           << '=' << config.memorySettings.maxTextures           << '\n'
             << "[physics]\n"
             << FixedUpdateIntervalKey   << '=' << config.physicsSettings.fixedUpdateInterval  << '\n'
             << WorldspaceExtentKey      << '=' << config.physicsSettings.worldspaceExtent     << '\n'
             << "[graphics]\n"
             << UseNativeResolutionKey   << '=' << config.graphicsSettings.useNativeResolution << '\n'
             << LaunchInFullscreenKey    << '=' << config.graphicsSettings.launchInFullscreen  << '\n'
             << ScreenWidthKey           << '=' << config.graphicsSettings.screenWidth         << '\n'
             << ScreenHeightKey          << '=' << config.graphicsSettings.screenHeight        << '\n'
             << TargetFpsKey             << '=' << config.graphicsSettings.targetFPS           << '\n'
             << NearClipKey              << '=' << config.graphicsSettings.nearClip            << '\n'
             << FarClipKey               << '=' << config.graphicsSettings.farClip             << '\n'
             << UseShadowsKey            << '=' << config.graphicsSettings.useShadows;
    }

    bool Validate(const Config& config)
    {
        return { (config.projectSettings.projectName != "") &&
                 (config.projectSettings.logFilePath != "") &&
                 (config.projectSettings.audioClipsPath != "") &&
                 (config.projectSettings.concaveCollidersPath != "") &&
                 (config.projectSettings.hullCollidersPath != "") &&
                 (config.projectSettings.meshesPath != "") &&
                 (config.projectSettings.shadersPath != "") &&
                 (config.projectSettings.texturesPath != "") &&
                 (config.projectSettings.cubeMapsPath != "") &&
                 (config.physicsSettings.fixedUpdateInterval > 0.0f) &&
                 (config.physicsSettings.worldspaceExtent > 0.0f) &&
                 (config.graphicsSettings.screenWidth != 0) &&
                 (config.graphicsSettings.screenHeight != 0) &&
                 (config.graphicsSettings.targetFPS != 0) &&
                 (config.graphicsSettings.frameUpdateInterval > 0.0f) &&
                 (config.graphicsSettings.nearClip > 0.0f) &&
                 (config.graphicsSettings.farClip > 0.0f)};
    }

    /* Internal function implementation */
    void LoadInternal(const std::string& configPath)
    {
        g_configPath = configPath;
        g_config = std::make_unique<Config>();
        nc::config::Read(configPath, MapKeyValue, g_config.get());
        if(!Validate(*g_config))
            throw NcError("Config validation failed");
    }
} // end namespace nc::config 