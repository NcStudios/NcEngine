#pragma once
#include "Component.h"
#include "directx/math/DirectXMath.h"

#include "graphics/d3dresource/GraphicsResource.h"

namespace nc::graphics { class Graphics; }

namespace nc
{
    //class Transform;

    class PointLight : public Component
    {
        public:
            PointLight();
            ~PointLight();
            PointLight(PointLight&&);
            PointLight& operator=(PointLight&&);
            PointLight(const PointLight&) = delete;
            PointLight& operator=(const PointLight&) = delete;

            void Set(DirectX::XMFLOAT3 pos, float radius = 0.5f);

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif

            void Bind(DirectX::FXMMATRIX view) noexcept(false);

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

            /** @todo took away ptr for debug */
            //mutable std::unique_ptr<PixelConstBuf> m_cBuf;
            mutable PixelConstBuf m_cBuf;
            Transform * m_transform;
    };
}