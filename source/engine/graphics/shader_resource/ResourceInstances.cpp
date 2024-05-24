#include "ResourceInstances.h"
#include "graphics/GraphicsConstants.h"
#include "graphics/GraphicsUtilities.h"
#include "graphics/shader_resource/ShaderResourceBus.h"

namespace nc::graphics
{
AssetResourcesConfig::AssetResourcesConfig(const config::MemorySettings& memorySettings)
    : maxTextures{memorySettings.maxTextures},
      maxCubeMaps{memorySettings.maxCubeMaps}
{}

AssetResources::AssetResources(AssetResourcesConfig config, ShaderResourceBus* resourceBus,  asset::NcAsset* ncAsset)
    : meshes{resourceBus->CreateMeshArrayBuffer()},
      onMeshArrayBufferUpdate{ncAsset->OnMeshUpdate().Connect(this, &AssetResources::ForwardMeshAssetData)},
      cubeMaps{resourceBus->CreateCubeMapArrayBuffer(config.maxCubeMaps, ShaderStage::Fragment, 4, 1)},
      onCubeMapArrayBufferUpdate{ncAsset->OnCubeMapUpdate().Connect(this, &AssetResources::ForwardCubeMapAssetData)},
      textures{resourceBus->CreateTextureArrayBuffer(config.maxTextures, ShaderStage::Fragment, 2, 1)},
      onTextureArrayBufferUpdate{ncAsset->OnTextureUpdate().Connect(this, &AssetResources::ForwardTextureAssetData)}{}

void AssetResources::ForwardMeshAssetData(const asset::MeshUpdateEventData& assetData)
{
    meshes.Initialize(assetData.vertices, assetData.indices);
}

void AssetResources::ForwardTextureAssetData(const asset::TextureUpdateEventData& assetData)
{
    switch (assetData.updateAction)
    {
        case asset::UpdateAction::Load:
        {
            textures.Add(assetData.data);
            break;
        }
        case asset::UpdateAction::Unload:
        {
            textures.Remove(assetData.data);
            break;
        }
        case asset::UpdateAction::UnloadAll:
        {
            textures.Clear();
            break;
        }
    }
}

void AssetResources::ForwardCubeMapAssetData(const asset::CubeMapUpdateEventData& assetData)
{
    switch (assetData.updateAction)
    {
        case asset::UpdateAction::Load:
        {
            cubeMaps.Add(assetData.data);
            break;
        }
        case asset::UpdateAction::Unload:
        {
            cubeMaps.Remove(assetData.data);
            break;
        }
        case asset::UpdateAction::UnloadAll:
        {
            cubeMaps.Clear();
            break;
        }
    }
}

PostProcessResources::PostProcessResources(uint32_t maxPointLights, ShaderResourceBus* resourceBus)
    : shadowMaps{resourceBus->CreatePPImageArrayBuffer(PostProcessImageType::ShadowMap, maxPointLights, ShaderStage::Fragment, 3u, 0u)}
{
}

SystemResourcesConfig::SystemResourcesConfig(const config::GraphicsSettings& graphicsSettings, const config::MemorySettings& memorySettings)
    : maxPointLights{memorySettings.maxPointLights},
      maxRenderers{memorySettings.maxRenderers},
      maxSkeletalAnimations{memorySettings.maxSkeletalAnimations},
      maxSpotLights{memorySettings.maxSpotLights},
      maxTextures{memorySettings.maxTextures},
      maxParticles{memorySettings.maxParticles},
      useShadows{graphicsSettings.useShadows}
{}

SystemResources::SystemResources(SystemResourcesConfig config,
                                Registry* registry,
                                ShaderResourceBus* resourceBus,
                                ModuleProvider modules,
                                SystemEvents& events,
                                std::function<graphics::Camera* ()> getCamera)
    : cameras{},
      environment{resourceBus},
      objects{resourceBus, config.maxRenderers},
      pointLights{resourceBus, config.maxPointLights, config.useShadows},
      skeletalAnimations{registry, resourceBus, config.maxSkeletalAnimations, modules.Get<asset::NcAsset>()->OnSkeletalAnimationUpdate(), modules.Get<asset::NcAsset>()->OnBoneUpdate()},
      spotLights{resourceBus, config.maxSpotLights, config.useShadows},
      widgets{},
      ui{registry->GetEcs(), modules, events},
      particleEmitters{registry, resourceBus, getCamera, config.maxParticles}
{
}
} // namespace nc::graphics
