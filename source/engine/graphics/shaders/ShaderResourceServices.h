#pragma once

#include "graphics/cubemaps/CubeMapManager.h"
#include "graphics/resources/EnvironmentDataManager.h"
#include "graphics/resources/ObjectDataManager.h"
#include "graphics/shaders/PointLightShaderResource.h"
#include "graphics/shaders/ShadowMapShaderResource.h"
#include "graphics/textures/TextureManager.h"
#include "ShaderDescriptorSets.h"
#include "config/Config.h"

namespace nc::graphics
{
class ShaderResourceServices
{
    public:
        ShaderResourceServices(vk::Device device, GpuAllocator* allocator, const config::MemorySettings& memorySettings, Vector2 dimensions)
            : m_shaderDescriptorSets{device},
              m_objectDataManager{0, allocator, &m_shaderDescriptorSets, memorySettings.maxRenderers},
              m_pointLightShaderResource{1, allocator, &m_shaderDescriptorSets, memorySettings.maxPointLights},
              m_textureManager{2, &m_shaderDescriptorSets, memorySettings.maxTextures},
              m_shadowMapShaderResource{device, 3, allocator, &m_shaderDescriptorSets, dimensions },
              m_cubeMapManager{device, 4, &m_shaderDescriptorSets, memorySettings.maxTextures}, // @todo make separate entry for cubeMaps
              m_environmentDataManager{5, allocator, &m_shaderDescriptorSets}
        {
            m_shaderDescriptorSets.CreateSet(BindFrequency::per_frame);
        }

        auto GetShadowMapShaderResource() noexcept -> ShadowMapShaderResource& { return m_shadowMapShaderResource; }
        auto GetDescriptorSets() noexcept -> ShaderDescriptorSets* { return &m_shaderDescriptorSets; }

    private:
        ShaderDescriptorSets m_shaderDescriptorSets;

        ObjectDataManager m_objectDataManager;                // BINDING SLOT 0
        PointLightShaderResource m_pointLightShaderResource;  // BINDING SLOT 1
        TextureManager m_textureManager;                      // BINDING SLOT 2
        ShadowMapShaderResource m_shadowMapShaderResource;    // BINDING SLOT 3
        CubeMapManager m_cubeMapManager;                      // BINDING SLOT 4
        EnvironmentDataManager m_environmentDataManager;      // BINDING SLOT 5
};
} // namespace nc::graphics