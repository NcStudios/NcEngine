#include "ShaderResources.h"
#include "graphics/shader_resource/ShaderResourceBus.h"

namespace nc::graphics
{
ShaderResources::ShaderResources(vk::Device device,
                                 ShaderDescriptorSets* shaderDescriptorSets,
                                 GpuAllocator* allocator,
                                 const config::MemorySettings& memorySettings,
                                 ShaderResourceBus& resourceBus)
    : textureShaderResource{2, shaderDescriptorSets, memorySettings.maxTextures},
      shadowMapShaderResource{3, device, shaderDescriptorSets, memorySettings.maxPointLights},
      cubeMapShaderResource{4, device, shaderDescriptorSets, memorySettings.maxTextures} // @todo make separate entry for cubeMaps
{
}
} // namespace nc::graphics
