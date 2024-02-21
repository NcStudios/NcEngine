#include "ShaderResources.h"
#include "graphics/shader_resource/EnvironmentData.h"
#include "graphics/shader_resource/ShaderResourceBus.h"

namespace
{
auto Connect(nc::Signal<const std::vector<nc::graphics::ObjectData>&>& signal, nc::graphics::ObjectDataShaderResource* manager)
{
    return signal.Connect([manager](auto&& data) { manager->Update(data); });
}

auto Connect(nc::Signal<const std::vector<nc::graphics::PointLightData>&>& signal, nc::graphics::PointLightShaderResource* manager)
{
    return signal.Connect([manager](auto&& data) { manager->Update(data); });
}

auto Connect(nc::Signal<const nc::graphics::EnvironmentData&>& signal, nc::graphics::EnvironmentDataShaderResource* manager)
{
    return signal.Connect([manager](auto&& data)
    {
        auto buffer = std::vector<nc::graphics::EnvironmentData>{};
        buffer.push_back(data);
        manager->Update(buffer);
    });
}

auto Connect(nc::Signal<const std::vector<nc::graphics::SkeletalAnimationData>&>& signal, nc::graphics::SkeletalAnimationShaderResource* manager)
{
    return signal.Connect([manager](auto&& data) { manager->Update(data); });
}
} // anonymous namespace

namespace nc::graphics
{
ShaderResources::ShaderResources(vk::Device device,
                                 ShaderDescriptorSets* shaderDescriptorSets,
                                 GpuAllocator* allocator,
                                 const config::MemorySettings& memorySettings,
                                 ShaderResourceBus& resourceBus)
    : objectDataShaderResource{0, allocator, shaderDescriptorSets, memorySettings.maxRenderers},
      pointLightShaderResource{1, allocator, shaderDescriptorSets, memorySettings.maxPointLights},
      textureShaderResource{2, shaderDescriptorSets, memorySettings.maxTextures},
      shadowMapShaderResource{3, device, shaderDescriptorSets, memorySettings.maxPointLights},
      cubeMapShaderResource{4, device, shaderDescriptorSets, memorySettings.maxCubeMaps},
      environmentDataShaderResource{5, allocator, shaderDescriptorSets},
      skeletalAnimationShaderResource{6, allocator, shaderDescriptorSets, memorySettings.maxSkeletalAnimations},
      objectDataConnection{::Connect(resourceBus.objectChannel, &objectDataShaderResource)},
      pointLightDataConnection{::Connect(resourceBus.pointLightChannel, &pointLightShaderResource)},
      environmentDataConnection{::Connect(resourceBus.environmentChannel, &environmentDataShaderResource)},
      skeletalAnimationDataConnection{::Connect(resourceBus.skeletalAnimationChannel, &skeletalAnimationShaderResource)}
{
    shaderDescriptorSets->CreateSet(BindFrequency::per_frame);
}
} // namespace nc::graphics
