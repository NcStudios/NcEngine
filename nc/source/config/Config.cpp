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
    struct Config
    {
        nc::config::ProjectSettings projectSettings;
        nc::config::MemorySettings memorySettings;
        nc::config::GraphicsSettings graphicsSettings;
        nc::config::PhysicsSettings physicsSettings;
    };

    std::unique_ptr<Config> g_config = nullptr;
    std::string g_configPath = "";

    // project
    const auto ProjectNameKey = std::string{"project_name"};
    const auto LogFilePathKey = std::string{"log_file_path"};

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
    const auto OctreeDensityThresholdKey = std::string{"octree_density_threshold"};
    const auto OctreeMinimumExtentKey = std::string{"octree_minimum_extent"};

    // graphics
    const auto UseNativeResolutionKey = std::string{"use_native_resolution"};
    const auto LaunchInFullscreenKey = std::string{"launch_fullscreen"};
    const auto ScreenWidthKey = std::string{"screen_width"};
    const auto ScreenHeightKey = std::string{"screen_height"};
    const auto TargetFpsKey = std::string{"target_fps"};
    const auto NearClipKey = std::string{"near_clip"};
    const auto FarClipKey = std::string{"far_clip"};
    const auto ShadersPathKey = std::string{"shaders_path"};

    void MapKeyValue(const std::string& key, const std::string& value, Config* out)
    {
        // project
        if (key == ProjectNameKey)
            out->projectSettings.projectName = value;
        else if (key == LogFilePathKey)
            out->projectSettings.logFilePath = value;
        
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
        else if (key == OctreeDensityThresholdKey)
            out->physicsSettings.octreeDensityThreshold = std::stoi(value);
        else if (key == OctreeMinimumExtentKey)
            out->physicsSettings.octreeMinimumExtent = std::stof(value);

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
        else if (key == ShadersPathKey)
            out->graphicsSettings.shadersPath = value;
        else
            throw std::runtime_error("config::MapKeyValue - Unknown key reading engine config");
    }
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

    /* Internal function implementation */
    void Load(const std::string& configPath)
    {
        g_configPath = configPath;
        g_config = std::make_unique<Config>();
        nc::config::Read(configPath, MapKeyValue, g_config.get());
        if(!Validate())
            throw std::runtime_error("config::Load - Failed to validate config");
    }

    bool Validate()
    {
        IF_THROW(!g_config, "config::Validate - No config loaded");
        return { (g_config->projectSettings.projectName != "") &&
                 (g_config->projectSettings.logFilePath != "") &&
                 (g_config->physicsSettings.fixedUpdateInterval > 0.0f) &&
                 (g_config->physicsSettings.worldspaceExtent > 0.0f) &&
                 (g_config->physicsSettings.octreeDensityThreshold > 0u) &&
                 (g_config->physicsSettings.octreeMinimumExtent > 0.0f) &&
                 (g_config->graphicsSettings.screenWidth != 0) &&
                 (g_config->graphicsSettings.screenHeight != 0) &&
                 (g_config->graphicsSettings.targetFPS != 0) &&
                 (g_config->graphicsSettings.frameUpdateInterval > 0.0f) &&
                 (g_config->graphicsSettings.nearClip > 0.0f) &&
                 (g_config->graphicsSettings.farClip > 0.0f) &&
                 (g_config->graphicsSettings.shadersPath != "")};
    }
} // end namespace nc::config 