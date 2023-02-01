#pragma once

#include "ecs/Registry.h"
#include "graphics/shaders/CubeMapShaderResource.h"
#include "graphics/shaders/EnvironmentDataShaderResource.h"
#include "graphics/shaders/ObjectDataShaderResource.h"
#include "graphics/shaders/PointLightShaderResource.h"
#include "graphics/shaders/ShadowMapShaderResource.h"
#include "graphics/shaders/TextureShaderResource.h"
#include "graphics/shaders/ShaderDescriptorSets.h"
#include "config/Config.h"

namespace nc::graphics
{
struct ShaderResources
{

        ShaderResources(vk::Device device, ShaderDescriptorSets* shaderDescriptorSets, Registry* registry, GpuAllocator* allocator, const config::MemorySettings& memorySettings, Vector2 dimensions)
            : objectDataShaderResource{0, allocator, shaderDescriptorSets, memorySettings.maxRenderers},
              pointLightShaderResource{1, allocator, shaderDescriptorSets, memorySettings.maxPointLights},
              textureShaderResource{2, shaderDescriptorSets, memorySettings.maxTextures},
              shadowMapShaderResource{3, device, shaderDescriptorSets, memorySettings.maxPointLights},
              cubeMapShaderResource{4, device, shaderDescriptorSets, memorySettings.maxTextures}, // @todo make separate entry for cubeMaps
              environmentDataShaderResource{5, allocator, shaderDescriptorSets}
        {
            shaderDescriptorSets->CreateSet(BindFrequency::per_frame);
        }

        ObjectDataShaderResource objectDataShaderResource;          
        PointLightShaderResource pointLightShaderResource;          
        TextureShaderResource textureShaderResource;                
        CubeMapShaderResource cubeMapShaderResource;                
        EnvironmentDataShaderResource environmentDataShaderResource;
        ShadowMapShaderResource shadowMapShaderResource;
};
} // namespace nc::graphics