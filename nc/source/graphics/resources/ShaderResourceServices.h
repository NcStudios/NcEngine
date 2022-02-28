#pragma once

#include "CubeMapManager.h"
#include "ObjectDataManager.h"
#include "PointLightManager.h"
#include "EnvironmentDataManager.h"
#include "ShaderDescriptorSets.h"
#include "ShadowMapManager.h"
#include "TextureManager.h"
#include "config/Config.h"

namespace nc::graphics
{
    class ShaderResourceServices
    {
        public:
            ShaderResourceServices(Graphics* graphics, const config::MemorySettings& memorySettings, Vector2 dimensions)
                : m_shaderDescriptorSets{graphics},
                  m_objectDataManager{0, graphics, &m_shaderDescriptorSets, memorySettings.maxRenderers},
                  m_pointLightManager{1, graphics, &m_shaderDescriptorSets, memorySettings.maxPointLights},
                  m_textureManager{2, graphics, &m_shaderDescriptorSets, memorySettings.maxTextures},
                  m_shadowMapManager{3,  graphics, &m_shaderDescriptorSets, dimensions },
                  m_cubeMapManager{4, graphics, &m_shaderDescriptorSets, memorySettings.maxTextures}, // @todo make separate entry for cubeMaps
                  m_environmentDataManager{5, graphics, &m_shaderDescriptorSets}
            {
                m_shaderDescriptorSets.CreateSet(BindFrequency::PerFrame);
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
}