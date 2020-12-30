#pragma once

#include "PointLight.h"

#include <memory>

namespace nc::graphics
{
    class Graphics;
    namespace d3dresource
    {
        template<typename T>
        class PixelConstantBuffer;
    }
}

namespace DirectX { struct XMMATRIX; }

namespace nc
{
    class Transform;

    class PointLightManager
    {
        public:
            static const uint32_t MAX_POINT_LIGHTS = 4u;

            PointLightManager();
            ~PointLightManager();
            uint32_t GetNextAvailableIndex();
            void AddPointLight(PointLight& pointLight, const DirectX::XMMATRIX& camMatrix);
            void Bind();

        private:
            uint32_t m_currentIndex;

            struct PointLightsArrayCBuf
            {
               PointLight::PointLightPixelCBuf PointLights[MAX_POINT_LIGHTS]; 
            } m_pointLightsArrayConstBufData;

            std::unique_ptr<graphics::d3dresource::PixelConstantBuffer<PointLightsArrayCBuf>> m_constantBuffer;
    };
}