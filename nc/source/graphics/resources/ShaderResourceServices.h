#pragma once

#include "ObjectDataManager.h"
#include "PointLIghtManager.h"
#include "TextureManager.h"
#include "config/Config.h"

namespace nc::graphics
{
    class ShaderResourceServices
    {
        public:
            ShaderResourceServices(Graphics* graphics, const config::MemorySettings& memorySettings)
                : m_objectDataManager{graphics, memorySettings.maxRenderers},
                  m_pointLightManager{graphics, memorySettings.maxPointLights},
                  m_textureManager{graphics, memorySettings.maxTextures}
            {
            }
        
        private:
            ObjectDataManager m_objectDataManager;
            PointLightManager m_pointLightManager;
            TextureManager m_textureManager;
    };
}