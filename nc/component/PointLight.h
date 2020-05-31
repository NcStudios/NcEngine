#pragma once
#include "Common.h"
#include "Component.h"
#include "DirectXMath.h"

#include "GraphicsResource.h"

namespace nc::graphics { class Graphics; }

namespace nc
{
    class PointLight : public Component
    {
        public:
            PointLight();

            void Set(graphics::Graphics * graphics, DirectX::XMFLOAT3 pos, float radius = 0.5f);

            #ifdef NC_DEBUG
            void EditorGuiElement() override;
            #endif

            void Bind(graphics::Graphics * graphics, DirectX::FXMMATRIX view) noexcept;

        private:
            struct PointLightCBuf
            {
                alignas(16)DirectX::XMFLOAT3 pos;
                alignas(16)DirectX::XMFLOAT3 ambient;
                alignas(16)DirectX::XMFLOAT3 diffuseColor;
                float diffuseIntensity;
                float attConst;
                float attLin;
                float attQuad;
            } m_constBufData;

            using PixelConstBuf = graphics::d3dresource::PixelConstantBuffer<PointLightCBuf>;
            mutable std::unique_ptr<PixelConstBuf> m_cBuf;
    };
}