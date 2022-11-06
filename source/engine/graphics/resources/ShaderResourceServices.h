#pragma once

#include "graphics/cubemaps/CubeMapManager.h"
#include "ObjectDataManager.h"
#include "PointLightManager.h"
#include "EnvironmentDataManager.h"
#include "ShaderDescriptorSets.h"
#include "ShadowMapManager.h"
#include "graphics/textures/TextureManager.h"
#include "config/Config.h"

namespace nc::graphics
{
class ShaderResourceServices
{
    public:
        ShaderResourceServices(vk::Device device, Graphics* graphics, GpuAllocator* allocator, const config::MemorySettings& memorySettings, Vector2 dimensions)
            : m_shaderDescriptorSets{device},
              m_objectDataManager{0, allocator, &m_shaderDescriptorSets, memorySettings.maxRenderers},
              m_pointLightManager{1, allocator, &m_shaderDescriptorSets, memorySettings.maxPointLights},
              m_textureManager{2, &m_shaderDescriptorSets, memorySettings.maxTextures},
              m_shadowMapManager{device, 3, allocator, &m_shaderDescriptorSets, dimensions },
              m_cubeMapManager{device, 4, &m_shaderDescriptorSets, memorySettings.maxTextures}, // @todo make separate entry for cubeMaps
              m_environmentDataManager{5, allocator, &m_shaderDescriptorSets}
        {
            m_shaderDescriptorSets.CreateSet(BindFrequency::per_frame);
        }

        auto GetShadowMapManager() noexcept -> ShadowMapManager& { return m_shadowMapManager; }
        auto GetDescriptorSets() noexcept -> ShaderDescriptorSets* { return &m_shaderDescriptorSets; }

    private:
        ShaderDescriptorSets m_shaderDescriptorSets;

        ObjectDataManager m_objectDataManager;              // BINDING SLOT 0
        PointLightManager m_pointLightManager;              // BINDING SLOT 1
        TextureManager m_textureManager;                    // BINDING SLOT 2
        ShadowMapManager m_shadowMapManager;                // BINDING SLOT 3
        CubeMapManager m_cubeMapManager;                    // BINDING SLOT 4
        EnvironmentDataManager m_environmentDataManager;    // BINDING SLOT 5
};
} // namespace nc::graphics