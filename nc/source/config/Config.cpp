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
    const auto ConfigPath = std::string{"nc/source/config/config.ini"};

    // project
    const auto ProjectNameKey = std::string{"project_name"};
    const auto LogFilePathKey = std::string{"log_file_path"};

    // memory
    const auto MaxDynamicCollidersKey = std::string{"max_dynamic_colliders"};
    const auto MaxStaticCollidersKey = std::string{"max_static_colliders"};
    const auto MaxNetworkDispatchersKey = std::string{"max_network_dispatchers"};
    const auto MaxRenderersKey = std::string{"max_renderers"};
    const auto MaxTransformsKey = std::string{"max_transforms"};

    // physics
    const auto FixedUpdateIntervalKey = std::string{"fixed_update_interval"};
    const auto OctreeDensityThresholdKey = std::string{"octree_density_threshold"};

    // graphics
    const auto UseNativeResolutionKey = std::string{"use_native_resolution"};
    const auto LaunchInFullscreenKey = std::string{"launch_fullscreen"};
    const auto ScreenWidthKey = std::string{"screen_width"};
    const auto ScreenHeightKey = std::string{"screen_height"};
    const auto TargetFpsKey = std::string{"target_fps"};
    const auto NearClipKey = std::string{"near_clip"};
    const auto FarClipKey = std::string{"far_clip"};
    const auto D3dShadersPathKey = std::string{"d3d_shaders_path"};
    const auto VulkanShadersPathKey = std::string{"vulkan_shaders_path"};

    void MapKeyValue(const std::string& key, const std::string& value, nc::config::Config& out)
    {
        // project
        if (key == ProjectNameKey)
            out.project.projectName = value;
        else if (key == LogFilePathKey)
            out.project.logFilePath = value;
        
        // memory
        else if(key == MaxDynamicCollidersKey)
            out.memory.maxDynamicColliders = std::stoi(value);
        else if(key == MaxStaticCollidersKey)
            out.memory.maxStaticColliders = std::stoi(value);
        else if(key == MaxNetworkDispatchersKey)
            out.memory.maxNetworkDispatchers = std::stoi(value);
        else if(key == MaxRenderersKey)
            out.memory.maxRenderers = std::stoi(value);
        else if(key == MaxTransformsKey)
            out.memory.maxTransforms = std::stoi(value);

        // physics
        else if (key == FixedUpdateIntervalKey)
            out.physics.fixedUpdateInterval = std::stod(value);
        else if (key == OctreeDensityThresholdKey)
            out.physics.octreeDensityThreshold = std::stoi(value);

        // graphics
        else if (key == UseNativeResolutionKey)
            out.graphics.useNativeResolution = std::stoi(value);
        else if (key == LaunchInFullscreenKey)
            out.graphics.launchInFullscreen = std::stoi(value);
        else if (key == ScreenWidthKey)
            out.graphics.screenWidth = std::stoi(value);
        else if (key == ScreenHeightKey)
            out.graphics.screenHeight = std::stoi(value);
        else if (key == TargetFpsKey)
        {
            out.graphics.targetFPS = std::stoi(value);
            out.graphics.frameUpdateInterval = 1.0 / static_cast<double>(out.graphics.targetFPS);
        }
        else if (key == NearClipKey)
            out.graphics.nearClip = std::stod(value);
        else if (key == FarClipKey)
            out.graphics.farClip = std::stod(value);
        else if (key == D3dShadersPathKey)
            out.graphics.d3dShadersPath = value;
        else if (key == VulkanShadersPathKey)
            out.graphics.vulkanShadersPath = value;
        else
            throw std::runtime_error("config::MapKeyValue - Unknown key reading engine config");
    };
} //end anonymous namespace

namespace nc::config
{
    std::unique_ptr<Config> g_instance = nullptr;

    /* Api function implementation */
    const Config& Get()
    {
        IF_THROW(!g_instance, "config::Get - No config loaded");
        return *g_instance;
    }

    /* Internal function implementation */
    void Load()
    {
        g_instance = std::make_unique<Config>();
        nc::config::Read(ConfigPath, MapKeyValue, *g_instance);
        if(!Validate())
            throw std::runtime_error("config::Load - Failed to validate config");
    }

    void Save()
    {
        IF_THROW(!g_instance, "config::Save - No config loaded");
        if (!Validate())
            throw std::runtime_error("config::Save - failed to validate config");

        std::ofstream outFile;
        outFile.open(ConfigPath);
        if(!outFile.is_open())
            throw std::runtime_error("config::Save - failed to open file");

        outFile << "[project]\n"
                << ProjectNameKey << INI_KEY_VALUE_DELIM << g_instance->project.projectName << '\n'
                << LogFilePathKey << INI_KEY_VALUE_DELIM << g_instance->project.logFilePath << '\n'
                << "[memory]\n"
                << MaxDynamicCollidersKey << INI_KEY_VALUE_DELIM << g_instance->memory.maxDynamicColliders << '\n'
                << MaxStaticCollidersKey << INI_KEY_VALUE_DELIM << g_instance->memory.maxStaticColliders << '\n'
                << MaxNetworkDispatchersKey << INI_KEY_VALUE_DELIM << g_instance->memory.maxNetworkDispatchers << '\n'
                << MaxRenderersKey << INI_KEY_VALUE_DELIM << g_instance->memory.maxRenderers << '\n'
                << MaxTransformsKey << INI_KEY_VALUE_DELIM << g_instance->memory.maxTransforms << '\n'
                << "[physics]\n"
                << FixedUpdateIntervalKey << INI_KEY_VALUE_DELIM << g_instance->physics.fixedUpdateInterval << '\n'
                << OctreeDensityThresholdKey << INI_KEY_VALUE_DELIM << g_instance->physics.octreeDensityThreshold << '\n'
                << "[graphics]\n"
                << UseNativeResolutionKey << INI_KEY_VALUE_DELIM << g_instance->graphics.useNativeResolution << '\n'
                << LaunchInFullscreenKey << INI_KEY_VALUE_DELIM << g_instance->graphics.launchInFullscreen << '\n'
                << ScreenWidthKey << INI_KEY_VALUE_DELIM << g_instance->graphics.screenWidth << '\n'
                << ScreenHeightKey << INI_KEY_VALUE_DELIM << g_instance->graphics.screenHeight << '\n'
                << TargetFpsKey << INI_KEY_VALUE_DELIM << g_instance->graphics.targetFPS << '\n'
                << NearClipKey << INI_KEY_VALUE_DELIM << g_instance->graphics.nearClip << '\n'
                << FarClipKey << INI_KEY_VALUE_DELIM << g_instance->graphics.farClip << '\n'
                << D3dShadersPathKey << INI_KEY_VALUE_DELIM << g_instance->graphics.d3dShadersPath << '\n'
                << VulkanShadersPathKey << INI_KEY_VALUE_DELIM << g_instance->graphics.vulkanShadersPath;

        outFile.close();
    }

    bool Validate()
    {
        IF_THROW(!g_instance, "config::Validate - No config loaded");
        return { (g_instance->project.projectName != "") &&
                 (g_instance->project.logFilePath != "") &&
                 (g_instance->physics.fixedUpdateInterval > 0.0f) &&
                 (g_instance->physics.octreeDensityThreshold > 0u) &&
                 (g_instance->graphics.screenWidth != 0) &&
                 (g_instance->graphics.screenHeight != 0) &&
                 (g_instance->graphics.targetFPS != 0) &&
                 (g_instance->graphics.frameUpdateInterval > 0.0f) &&
                 (g_instance->graphics.nearClip > 0.0f) &&
                 (g_instance->graphics.farClip > 0.0f) &&
                 (g_instance->graphics.d3dShadersPath != "") &&
                 (g_instance->graphics.vulkanShadersPath != "")};
    }
} // end namespace nc::config 