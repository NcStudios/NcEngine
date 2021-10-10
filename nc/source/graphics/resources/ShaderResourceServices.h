#pragma once

#include "ObjectData.h"
#include "PointLIghtManager.h"
#include "TextureManager.h"
#include "config/Config.h"

namespace nc::graphics
{
    class ShaderResourceServices
    {
        public:
            ShaderResourceServices(Graphics* graphics, const config::MemorySettings& memorySettings)
                : m_objectDataManager{},
                  m_pointLightManager{memorySettings.maxPointLights},
                  m_textureManager{graphics, memorySettings.maxTextures}
            {
                m_objectDataManager.Initialize(graphics);
                m_pointLightManager.Initialize(graphics);
            }
        
        private:
            ObjectDataManager m_objectDataManager;
            PointLightManager m_pointLightManager;
            TextureManager m_textureManager;
    };
}