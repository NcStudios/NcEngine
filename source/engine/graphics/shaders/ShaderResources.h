#pragma once

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
        ShaderResources(vk::Device device, GpuAllocator* allocator, const config::MemorySettings& memorySettings, Vector2 dimensions)
            : m_shaderDescriptorSets{device},
              m_objectDataShaderResource{0, allocator, &m_shaderDescriptorSets, memorySettings.maxRenderers},
              m_pointLightShaderResource{1, allocator, &m_shaderDescriptorSets, memorySettings.maxPointLights},
              m_textureShaderResource{2, &m_shaderDescriptorSets, memorySettings.maxTextures},
              m_shadowMapShaderResource{device, 3, allocator, &m_shaderDescriptorSets, dimensions, 2},
              m_cubeMapShaderResource{device, 4, &m_shaderDescriptorSets, memorySettings.maxTextures}, // @todo make separate entry for cubeMaps
              m_environmentDataShaderResource{5, allocator, &m_shaderDescriptorSets}
        {
            m_shaderDescriptorSets.CreateSet(BindFrequency::per_frame);
        }

        auto GetShadowMapShaderResource() noexcept -> ShadowMapShaderResource& { return m_shadowMapShaderResource; }
        auto GetDescriptorSets() noexcept -> ShaderDescriptorSets* { return &m_shaderDescriptorSets; }

    private:
        ShaderDescriptorSets m_shaderDescriptorSets;

        ObjectDataShaderResource m_objectDataShaderResource;           // BINDING SLOT 0
        PointLightShaderResource m_pointLightShaderResource;           // BINDING SLOT 1
        TextureShaderResource m_textureShaderResource;                 // BINDING SLOT 2
        ShadowMapShaderResource m_shadowMapShaderResource;             // BINDING SLOT 3
        CubeMapShaderResource m_cubeMapShaderResource;                 // BINDING SLOT 4
        EnvironmentDataShaderResource m_environmentDataShaderResource; // BINDING SLOT 5
};
} // namespace nc::graphics