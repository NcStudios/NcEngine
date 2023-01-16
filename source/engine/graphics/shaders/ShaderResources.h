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
class ShaderResources
{
    public:
        ShaderResources(vk::Device device, Registry* registry, GpuAllocator* allocator, const config::MemorySettings& memorySettings, Vector2 dimensions)
            : m_shaderDescriptorSets{device},
              m_objectDataShaderResource{0, allocator, &m_shaderDescriptorSets, memorySettings.maxRenderers},
              m_pointLightShaderResource{1, allocator, &m_shaderDescriptorSets, memorySettings.maxPointLights},
              m_textureShaderResource{2, &m_shaderDescriptorSets, memorySettings.maxTextures},
              m_shadowMapShaderResource{3, device, registry, allocator, &m_shaderDescriptorSets, dimensions},
              m_cubeMapShaderResource{4, device, &m_shaderDescriptorSets, memorySettings.maxTextures}, // @todo make separate entry for cubeMaps
              m_environmentDataShaderResource{5, allocator, &m_shaderDescriptorSets}
        {
            m_shaderDescriptorSets.CreateSet(BindFrequency::per_frame);
        }

        auto GetShadowMapShaderResource() noexcept -> ShadowMapShaderResource& { return m_shadowMapShaderResource; }
        auto GetDescriptorSets() noexcept -> ShaderDescriptorSets* { return &m_shaderDescriptorSets; }

    private:
        ShaderDescriptorSets m_shaderDescriptorSets;

        ObjectDataShaderResource m_objectDataShaderResource;          
        PointLightShaderResource m_pointLightShaderResource;          
        TextureShaderResource m_textureShaderResource;                
        CubeMapShaderResource m_cubeMapShaderResource;                
        EnvironmentDataShaderResource m_environmentDataShaderResource;
        ShadowMapShaderResource m_shadowMapShaderResource;
};
} // namespace nc::graphics