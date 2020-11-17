#pragma once

#include "PointLight.h"

namespace nc::graphics { class Graphics; }
namespace DirectX { struct XMMATRIX; }

namespace nc
{
    class Transform;

    class PointLightManager
    {
        public:

            PointLightManager() = default;
            ~PointLightManager() = default;
            static const uint32_t MAX_POINT_LIGHTS = 4u;
            uint32_t GetNextAvailableIndex();
            void AddPointLight(PointLight& pointLight, const DirectX::XMMATRIX& camMatrix);
            void Bind();

        private:

            uint32_t m_currentIndex;
            struct PointLightsArrayCBuf
            {
               PointLight::PointLightPixelCBuf PointLights[MAX_POINT_LIGHTS]; 
            } m_pointLightsArrayConstBufData;
    };
}