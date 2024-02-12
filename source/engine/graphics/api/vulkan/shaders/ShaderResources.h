#pragma once

#include "CubeMapShaderResource.h"
#include "ShaderDescriptorSets.h"
#include "ShadowMapShaderResource.h"
#include "TextureShaderResource.h"
#include "config/Config.h"
#include "utility/Signal.h"

namespace nc::graphics
{
struct ShaderResourceBus;

struct ShaderResources
{
    ShaderResources(vk::Device device,
                    ShaderDescriptorSets* shaderDescriptorSets,
                    GpuAllocator* allocator,
                    const config::MemorySettings& memorySettings,
                    ShaderResourceBus& resourceBus);

    TextureShaderResource textureShaderResource;
    ShadowMapShaderResource shadowMapShaderResource;
    CubeMapShaderResource cubeMapShaderResource;
};
} // namespace nc::graphics
