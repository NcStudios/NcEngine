#pragma once

#include "graphics/shader_resource/CubeMapArrayBufferHandle.h"
#include "graphics/shader_resource/MeshArrayBufferHandle.h"
#include "graphics/shader_resource/TextureArrayBufferHandle.h"
#include "graphics/system/CameraSystem.h"
#include "graphics/system/EnvironmentSystem.h"
#include "graphics/system/ObjectSystem.h"
#include "graphics/system/ParticleEmitterSystem.h"
#include "graphics/system/PointLightSystem.h"
#include "graphics/system/SkeletalAnimationSystem.h"
#include "graphics/system/UISystem.h"
#include "graphics/system/WidgetSystem.h"

#include "ncengine/asset/NcAsset.h"
#include "ncengine/config/Config.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/module/ModuleProvider.h"

namespace config
{
struct GraphicsSettings;
struct MemorySettings;
struct ProjectSettings;
} // namespace config

namespace nc::graphics
{
struct AssetResourcesConfig
{
    AssetResourcesConfig(const config::MemorySettings& memorySettings);
    uint32_t maxTextures;
    uint32_t maxCubeMaps;
};

struct AssetResources
{
    AssetResources(AssetResourcesConfig config, ShaderResourceBus* resourceBus, ModuleProvider modules);

    MeshArrayBufferHandle meshes;
    nc::Connection<const asset::MeshUpdateEventData&> onMeshArrayBufferUpdate;
    void ForwardMeshAssetData(const asset::MeshUpdateEventData& assetData);

    CubeMapArrayBufferHandle cubeMaps;
    nc::Connection<const asset::CubeMapUpdateEventData&> onCubeMapArrayBufferUpdate;
    void ForwardCubeMapAssetData(const asset::CubeMapUpdateEventData& assetData);

    TextureArrayBufferHandle textures;
    nc::Connection<const asset::TextureUpdateEventData&> onTextureArrayBufferUpdate;
    void ForwardTextureAssetData(const asset::TextureUpdateEventData& assetData);
};

struct SystemResourcesConfig
{
    SystemResourcesConfig(const config::GraphicsSettings& graphicsSettings,
                          const config::MemorySettings& memorySettings);
    uint32_t maxPointLights;
    uint32_t maxRenderers;
    uint32_t maxSkeletalAnimations;
    uint32_t maxTextures;
    bool useShadows;
};

struct SystemResources
{
    SystemResources(SystemResourcesConfig config, 
                    Registry* registry,
                    ShaderResourceBus* resourceBus,
                    ModuleProvider modules,
                    SystemEvents& events,
                    std::function<graphics::Camera* ()> getCamera);
    CameraSystem cameras;
    EnvironmentSystem environment;
    ObjectSystem objects;
    PointLightSystem pointLights;
    SkeletalAnimationSystem skeletalAnimations;
    WidgetSystem widgets;
    UISystem ui;
    ParticleEmitterSystem particleEmitters;
};
} // namespace nc::graphics
