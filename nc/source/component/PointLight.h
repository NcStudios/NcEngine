#pragma once
#include "Component.h"
#include "directx/math/DirectXMath.h"

#include "graphics/d3dresource/GraphicsResource.h"
#include "NcConfig.h"

namespace nc::graphics { class Graphics; }

namespace nc
{
    class Transform;

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

            void SetPositionFromCameraProjection(DirectX::FXMMATRIX view) noexcept(false);

            struct PointLightPixelCBuf
            {
                alignas(16)DirectX::XMFLOAT3 pos;
                alignas(16)DirectX::XMFLOAT3 ambient;
                alignas(16)DirectX::XMFLOAT3 diffuseColor;
                float diffuseIntensity;
                float attConst;
                float attLin;
                float attQuad;
            } m_pixelConstBufData;

            alignas(16)DirectX::XMFLOAT3 projectedPos;

        private:

            Transform * m_transform;
    };
}