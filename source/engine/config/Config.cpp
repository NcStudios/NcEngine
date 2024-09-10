#include "ncengine/config/Config.h"
#include "ConfigInternal.h"

#include "ncutility/NcError.h"

#include <algorithm>
#include <charconv>
#include <concepts>
#include <fstream>
#include <limits>
#include <unordered_map>

namespace
{
auto g_config = nc::config::Config{};

using namespace std::literals;

// project
constexpr auto ProjectNameKey = "project_name"sv;
constexpr auto LogFilePathKey = "log_file_path"sv;
constexpr auto LogMaxFileSizeKey = "log_max_file_size"sv;

// engine
constexpr auto TimeStepKey = "time_step"sv;
constexpr auto MaxTimeStepKey = "max_time_step"sv;
constexpr auto ThreadCountKey = "thread_count"sv;
constexpr auto BuildTasksOnInitKey = "build_tasks_on_init"sv;

// asset
constexpr auto AudioClipsPathKey = "audio_clips_path"sv;
constexpr auto ConcaveCollidersKey = "concave_colliders_path"sv;
constexpr auto CubeMapsPathKey = "cubemaps_path"sv;
constexpr auto HullCollidersKey = "hull_colliders_path"sv;
constexpr auto MeshesPathKey = "meshes_path"sv;
constexpr auto ShadersPathKey = "shaders_path"sv;
constexpr auto SkeletalAnimationsPathKey = "skeletal_animations_path"sv;
constexpr auto TexturesPathKey = "textures_path"sv;
constexpr auto FontsPathKey = "fonts_path"sv;

// memory
constexpr auto MaxRigidBodiesKey = "max_rigid_bodies"sv;
constexpr auto MaxNetworkDispatchersKey = "max_network_dispatchers"sv;
constexpr auto MaxParticleEmittersKey = "max_particle_emitters"sv;
constexpr auto MaxRenderersKey = "max_renderers"sv;
constexpr auto MaxTransformsKey = "max_transforms"sv;
constexpr auto MaxPointLightsKey = "max_point_lights"sv;
constexpr auto MaxSkeletalAnimationsKey = "max_skeletal_animations"sv;
constexpr auto MaxSpotLightsKey = "max_spot_lights"sv;
constexpr auto MaxTexturesKey = "max_textures"sv;
constexpr auto MaxCubeMapsKey = "max_cubemaps"sv;
constexpr auto MaxParticlesKey = "max_particles"sv;

// physics
constexpr auto PhysicsEnabledKey = "physics_enabled"sv;
constexpr auto TempAllocatorSizeKey = "temp_allocator_size"sv;
constexpr auto MaxBodyPairsKey = "max_body_pairs"sv;
constexpr auto MaxContactsKey = "max_contacts"sv;
constexpr auto VelocityStepsKey = "velocity_steps"sv;
constexpr auto PositionStepsKey = "position_steps"sv;
constexpr auto BaumgarteStabilizationKey = "baumgarte_stabilization"sv;
constexpr auto SpeculativeContactDistanceKey = "speculative_contact_distance"sv;
constexpr auto PenetrationSlopKey = "penetration_slop"sv;
constexpr auto TimeBeforeSleepKey = "time_before_sleep"sv;
constexpr auto SleepThresholdKey = "sleep_threshold"sv;

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
constexpr auto BufferFramesKey = "buffer_frames"sv;

auto ValidateBufferFrames(unsigned frames)
{
    switch(frames)
    {
        case 16:   [[fallthrough]];
        case 32:   [[fallthrough]];
        case 64:   [[fallthrough]];
        case 128:  [[fallthrough]];
        case 512:  [[fallthrough]];
        case 1024: [[fallthrough]];
        case 2048: return true;
        default:   return false;
    }
}

auto ValidatePhysicsSettings(const nc::config::PhysicsSettings& settings) -> bool
{
    return settings.velocitySteps >= 2u &&
           settings.baumgarteStabilization >= 0.0f &&
           settings.baumgarteStabilization <= 1.0f;
}

auto TrimWhiteSpace(const std::string& str) -> std::string
{
    const auto notSpace = [](char c) { return !std::isspace(c); };
    auto out = str;
    out.erase(out.begin(), std::find_if(out.begin(), out.end(), notSpace));
    out.erase(std::find_if(out.rbegin(), out.rend(), notSpace).base(), out.end());
    return out;
}

auto ReadConfigMap(std::istream& stream) -> std::unordered_map<std::string, std::string>
{
    constexpr auto iniSkipChars = std::string_view{";#["};
    constexpr auto delim = '=';
    auto map = std::unordered_map<std::string, std::string>{};
    auto line = std::string{};
    while (std::getline(stream, line))
    {
        line = TrimWhiteSpace(line);
        if (line.empty() || line.find_first_of(iniSkipChars) == 0)
        {
            continue;
        }

        const auto delimPos = line.find(delim);
        if (delimPos == std::string::npos)
        {
            throw nc::NcError(fmt::format("Unexpected contents in config file: {}", line));
        }

        map.emplace
        (
            TrimWhiteSpace(line.substr(0, delimPos)),
            TrimWhiteSpace(line.substr(delimPos + 1))
        );
    }

    return map;
}

template <class T>
auto ToArithmeticType(const std::string& str) -> T
{
    auto out = T{};
    const auto [_, ec] = std::from_chars(str.data(), str.data() + str.size(), out);
    if (ec != std::errc{})
    {
        throw nc::NcError(fmt::format("Failed to convert config value: {}", str));
    }

    return out;
}

template<class T>
void ParseValueIfExists(T& out, std::string_view key, const std::unordered_map<std::string, std::string>& kvPairs)
{
    const auto keyString = std::string{key};
    if (!kvPairs.contains(keyString))
    {
        return;
    }

    const auto& valueString = kvPairs.at(keyString);
    if constexpr (std::same_as<T, std::string>)
    {
        out = valueString;
    }
    else if constexpr (std::same_as<T, bool>)
    {
        out = static_cast<bool>(ToArithmeticType<unsigned>(valueString));
    }
    else
    {
        out = ToArithmeticType<T>(valueString);
    }
}

template <class Struct_t>
auto BuildFromConfigMap(const std::unordered_map<std::string, std::string>& kvPairs) -> Struct_t
{
    auto out = Struct_t{};

    if constexpr (std::same_as<Struct_t, nc::config::ProjectSettings>)
    {
        ParseValueIfExists(out.projectName, ProjectNameKey, kvPairs);
        ParseValueIfExists(out.logFilePath, LogFilePathKey, kvPairs);
        ParseValueIfExists(out.logMaxFileSize, LogMaxFileSizeKey, kvPairs);
    }
    else if constexpr (std::same_as<Struct_t, nc::config::EngineSettings>)
    {
        ParseValueIfExists(out.timeStep, TimeStepKey, kvPairs);
        ParseValueIfExists(out.maxTimeStep, MaxTimeStepKey, kvPairs);
        ParseValueIfExists(out.threadCount, ThreadCountKey, kvPairs);
        ParseValueIfExists(out.buildTasksOnInit, BuildTasksOnInitKey, kvPairs);
    }
    else if constexpr (std::same_as<Struct_t, nc::config::AssetSettings>)
    {
        ParseValueIfExists(out.audioClipsPath, AudioClipsPathKey, kvPairs);
        ParseValueIfExists(out.concaveCollidersPath, ConcaveCollidersKey, kvPairs);
        ParseValueIfExists(out.cubeMapsPath, CubeMapsPathKey, kvPairs);
        ParseValueIfExists(out.hullCollidersPath, HullCollidersKey, kvPairs);
        ParseValueIfExists(out.meshesPath, MeshesPathKey, kvPairs);
        ParseValueIfExists(out.shadersPath, ShadersPathKey, kvPairs);
        ParseValueIfExists(out.skeletalAnimationsPath, SkeletalAnimationsPathKey, kvPairs);
        ParseValueIfExists(out.texturesPath, TexturesPathKey, kvPairs);
        ParseValueIfExists(out.fontsPath, FontsPathKey, kvPairs);
    }
    else if constexpr (std::same_as<Struct_t, nc::config::MemorySettings>)
    {
        ParseValueIfExists(out.maxRigidBodies, MaxRigidBodiesKey, kvPairs);
        ParseValueIfExists(out.maxNetworkDispatchers, MaxNetworkDispatchersKey, kvPairs);
        ParseValueIfExists(out.maxParticleEmitters, MaxParticleEmittersKey, kvPairs);
        ParseValueIfExists(out.maxRenderers, MaxRenderersKey, kvPairs);
        ParseValueIfExists(out.maxTransforms, MaxTransformsKey, kvPairs);
        ParseValueIfExists(out.maxPointLights, MaxPointLightsKey, kvPairs);
        ParseValueIfExists(out.maxSkeletalAnimations, MaxSkeletalAnimationsKey, kvPairs);
        ParseValueIfExists(out.maxSpotLights, MaxSpotLightsKey, kvPairs);
        ParseValueIfExists(out.maxTextures, MaxTexturesKey, kvPairs);
        ParseValueIfExists(out.maxCubeMaps, MaxCubeMapsKey, kvPairs);
        ParseValueIfExists(out.maxParticles, MaxParticlesKey, kvPairs);
    }
    else if constexpr (std::same_as<Struct_t, nc::config::GraphicsSettings>)
    {
        ParseValueIfExists(out.enabled, GraphicsEnabledKey, kvPairs);
        ParseValueIfExists(out.useNativeResolution, UseNativeResolutionKey, kvPairs);
        ParseValueIfExists(out.launchInFullscreen, LaunchInFullscreenKey, kvPairs);
        ParseValueIfExists(out.screenWidth, ScreenWidthKey, kvPairs);
        ParseValueIfExists(out.screenHeight, ScreenHeightKey, kvPairs);
        ParseValueIfExists(out.targetFPS, TargetFpsKey, kvPairs);
        ParseValueIfExists(out.nearClip, NearClipKey, kvPairs);
        ParseValueIfExists(out.farClip, FarClipKey, kvPairs);
        ParseValueIfExists(out.useShadows, UseShadowsKey, kvPairs);
        ParseValueIfExists(out.antialiasing, AntialiasingKey, kvPairs);
        ParseValueIfExists(out.useValidationLayers, UseValidationLayersKey, kvPairs);
    }
    else if constexpr (std::same_as<Struct_t, nc::config::PhysicsSettings>)
    {
        ParseValueIfExists(out.enabled, PhysicsEnabledKey, kvPairs);
        ParseValueIfExists(out.tempAllocatorSize, TempAllocatorSizeKey, kvPairs);
        ParseValueIfExists(out.maxBodyPairs, MaxBodyPairsKey, kvPairs);
        ParseValueIfExists(out.maxContacts, MaxContactsKey, kvPairs);
        ParseValueIfExists(out.velocitySteps, VelocityStepsKey, kvPairs);
        ParseValueIfExists(out.positionSteps, PositionStepsKey, kvPairs);
        ParseValueIfExists(out.baumgarteStabilization, BaumgarteStabilizationKey, kvPairs);
        ParseValueIfExists(out.speculativeContactDistance, SpeculativeContactDistanceKey, kvPairs);
        ParseValueIfExists(out.penetrationSlop, PenetrationSlopKey, kvPairs);
        ParseValueIfExists(out.timeBeforeSleep, TimeBeforeSleepKey, kvPairs);
        ParseValueIfExists(out.sleepThreshold, SleepThresholdKey, kvPairs);
    }
    else if constexpr (std::same_as<Struct_t, nc::config::AudioSettings>)
    {
        ParseValueIfExists(out.enabled, AudioEnabledKey, kvPairs);
        ParseValueIfExists(out.bufferFrames, BufferFramesKey, kvPairs);
    }

    return out;
}

template<class T>
void WriteKVPair(std::ostream& stream, std::string_view key, const T& value)
{
    stream << key << '=' << value << '\n';
}
} // anonymous namespace

namespace nc::config
{
const ProjectSettings& GetProjectSettings()
{
    return g_config.projectSettings;
}

const EngineSettings& GetEngineSettings()
{
    return g_config.engineSettings;
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

auto Load(const std::filesystem::path& path) -> Config
{
    auto file = std::ifstream{path};
    if (!file.is_open())
    {
        throw NcError{fmt::format("Failed to open config file: {}", path.string())};
    }

    return Read(file);
}

auto Read(std::istream& stream) -> Config
{
    const auto kvPairs = ::ReadConfigMap(stream);
    return Config
    {
        ::BuildFromConfigMap<ProjectSettings>(kvPairs),
        ::BuildFromConfigMap<EngineSettings>(kvPairs),
        ::BuildFromConfigMap<AssetSettings>(kvPairs),
        ::BuildFromConfigMap<MemorySettings>(kvPairs),
        ::BuildFromConfigMap<GraphicsSettings>(kvPairs),
        ::BuildFromConfigMap<PhysicsSettings>(kvPairs),
        ::BuildFromConfigMap<AudioSettings>(kvPairs)
    };
}

void Save(const std::filesystem::path& path, const Config& config)
{
    auto file = std::ofstream{path};
    if(!file.is_open())
    {
        throw NcError{fmt::format("Failed to open config file: {}", path.string())};
    }

    Write(file, config);
}

void Write(std::ostream& stream, const Config& config, bool writeSections)
{
    if (writeSections) stream << "[project_settings]\n";
    ::WriteKVPair(stream, ProjectNameKey, config.projectSettings.projectName);
    ::WriteKVPair(stream, LogFilePathKey, config.projectSettings.logFilePath);
    ::WriteKVPair(stream, LogMaxFileSizeKey, config.projectSettings.logMaxFileSize);

    if (writeSections) stream << "[engine_settings]\n";
    ::WriteKVPair(stream, TimeStepKey, config.engineSettings.timeStep);
    ::WriteKVPair(stream, MaxTimeStepKey, config.engineSettings.maxTimeStep);
    ::WriteKVPair(stream, ThreadCountKey, config.engineSettings.threadCount);
    ::WriteKVPair(stream, BuildTasksOnInitKey, config.engineSettings.buildTasksOnInit);

    if (writeSections) stream << "[asset_settings]\n";
    ::WriteKVPair(stream, AudioClipsPathKey, config.assetSettings.audioClipsPath);
    ::WriteKVPair(stream, ConcaveCollidersKey, config.assetSettings.concaveCollidersPath);
    ::WriteKVPair(stream, CubeMapsPathKey, config.assetSettings.cubeMapsPath);
    ::WriteKVPair(stream, HullCollidersKey, config.assetSettings.hullCollidersPath);
    ::WriteKVPair(stream, MeshesPathKey, config.assetSettings.meshesPath);
    ::WriteKVPair(stream, ShadersPathKey, config.assetSettings.shadersPath);
    ::WriteKVPair(stream, SkeletalAnimationsPathKey, config.assetSettings.skeletalAnimationsPath);
    ::WriteKVPair(stream, TexturesPathKey, config.assetSettings.texturesPath);
    ::WriteKVPair(stream, FontsPathKey, config.assetSettings.fontsPath);

    if (writeSections) stream << "[memory_settings]\n";
    ::WriteKVPair(stream, MaxRigidBodiesKey, config.memorySettings.maxRigidBodies);
    ::WriteKVPair(stream, MaxNetworkDispatchersKey, config.memorySettings.maxNetworkDispatchers);
    ::WriteKVPair(stream, MaxParticleEmittersKey, config.memorySettings.maxParticleEmitters);
    ::WriteKVPair(stream, MaxPointLightsKey, config.memorySettings.maxPointLights);
    ::WriteKVPair(stream, MaxRenderersKey, config.memorySettings.maxRenderers);
    ::WriteKVPair(stream, MaxTransformsKey, config.memorySettings.maxTransforms);
    ::WriteKVPair(stream, MaxSkeletalAnimationsKey, config.memorySettings.maxSkeletalAnimations);
    ::WriteKVPair(stream, MaxSpotLightsKey, config.memorySettings.maxSpotLights);
    ::WriteKVPair(stream, MaxTexturesKey, config.memorySettings.maxTextures);
    ::WriteKVPair(stream, MaxCubeMapsKey, config.memorySettings.maxCubeMaps);
    ::WriteKVPair(stream, MaxParticlesKey, config.memorySettings.maxParticles);

    if (writeSections) stream << "[physics_settings]\n";
    ::WriteKVPair(stream, PhysicsEnabledKey, config.physicsSettings.enabled);
    ::WriteKVPair(stream, TempAllocatorSizeKey, config.physicsSettings.tempAllocatorSize);
    ::WriteKVPair(stream, MaxBodyPairsKey, config.physicsSettings.maxBodyPairs);
    ::WriteKVPair(stream, MaxContactsKey, config.physicsSettings.maxContacts);
    ::WriteKVPair(stream, VelocityStepsKey, config.physicsSettings.velocitySteps);
    ::WriteKVPair(stream, PositionStepsKey, config.physicsSettings.positionSteps);
    ::WriteKVPair(stream, BaumgarteStabilizationKey, config.physicsSettings.baumgarteStabilization);
    ::WriteKVPair(stream, SpeculativeContactDistanceKey, config.physicsSettings.speculativeContactDistance);
    ::WriteKVPair(stream, PenetrationSlopKey, config.physicsSettings.penetrationSlop);
    ::WriteKVPair(stream, TimeBeforeSleepKey, config.physicsSettings.timeBeforeSleep);
    ::WriteKVPair(stream, SleepThresholdKey, config.physicsSettings.sleepThreshold);

    if (writeSections) stream << "[graphics_settings]\n";
    ::WriteKVPair(stream, GraphicsEnabledKey, config.graphicsSettings.enabled);
    ::WriteKVPair(stream, UseNativeResolutionKey, config.graphicsSettings.useNativeResolution);
    ::WriteKVPair(stream, LaunchInFullscreenKey, config.graphicsSettings.launchInFullscreen);
    ::WriteKVPair(stream, ScreenWidthKey, config.graphicsSettings.screenWidth);
    ::WriteKVPair(stream, ScreenHeightKey, config.graphicsSettings.screenHeight);
    ::WriteKVPair(stream, TargetFpsKey, config.graphicsSettings.targetFPS);
    ::WriteKVPair(stream, NearClipKey, config.graphicsSettings.nearClip);
    ::WriteKVPair(stream, FarClipKey, config.graphicsSettings.farClip);
    ::WriteKVPair(stream, UseShadowsKey, config.graphicsSettings.useShadows);
    ::WriteKVPair(stream, AntialiasingKey, config.graphicsSettings.antialiasing);
    ::WriteKVPair(stream, UseValidationLayersKey, config.graphicsSettings.useValidationLayers);

    if (writeSections) stream << "[audio_settings]\n";
    ::WriteKVPair(stream, AudioEnabledKey, config.audioSettings.enabled);
    ::WriteKVPair(stream, BufferFramesKey, config.audioSettings.bufferFrames);
}

bool Validate(const Config& config)
{
    return (config.projectSettings.projectName != "") &&
           (config.projectSettings.logFilePath != "") &&
           (config.projectSettings.logMaxFileSize > 0) &&
           (config.engineSettings.timeStep >= 0.0f) &&
           (config.engineSettings.maxTimeStep > 0.0f) &&
           (config.assetSettings.audioClipsPath != "") &&
           (config.assetSettings.concaveCollidersPath != "") &&
           (config.assetSettings.hullCollidersPath != "") &&
           (config.assetSettings.meshesPath != "") &&
           (config.assetSettings.shadersPath != "") &&
           (config.assetSettings.texturesPath != "") &&
           (config.assetSettings.cubeMapsPath != "") &&
           (config.assetSettings.fontsPath != "") &&
           (config.graphicsSettings.screenWidth != 0) &&
           (config.graphicsSettings.screenHeight != 0) &&
           (config.graphicsSettings.targetFPS != 0) &&
           (config.graphicsSettings.nearClip > 0.0f) &&
           (config.graphicsSettings.farClip > 0.0f) &&
           (config.graphicsSettings.antialiasing > 0) &&
           ValidatePhysicsSettings(config.physicsSettings) &&
           ValidateBufferFrames(config.audioSettings.bufferFrames);
}

// Implementation from ConfigInternal.h
void SetConfig(const Config& config)
{
    g_config = config;
}
} // end namespace nc::config
