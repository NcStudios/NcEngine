#pragma once

#include "ObjectDataManager.h"
#include "PointLightManager.h"
#include "ShadowMapManager.h"
#include "TextureManager.h"
#include "config/Config.h"

namespace nc::graphics
{
    class ShaderResourceServices
    {
        public:
            ShaderResourceServices(Graphics* graphics, const config::MemorySettings& memorySettings, Vector2 dimensions)
                : m_objectDataManager{graphics, memorySettings.maxRenderers},
                  m_pointLightManager{graphics, memorySettings.maxPointLights},
                  m_shadowMapManager{graphics, dimensions},
                  m_textureManager{graphics, memorySettings.maxTextures},
                  m_environmentDataManager{}
            {
            }
        
            auto GetShadowMapManager() noexcept -> ShadowMapManager& { return m_shadowMapManager; }

        private:
            ObjectDataManager m_objectDataManager;
            PointLightManager m_pointLightManager;
            ShadowMapManager m_shadowMapManager;
            TextureManager m_textureManager;
            EnvironmentDataManager m_environmentDataManager;
    };
}