#pragma once
#include "Component.h"
#include "directx/math/DirectXMath.h"

#include "graphics/d3dresource/GraphicsResource.h"
#include "config/Config.h"

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

            void SetPositionFromCameraProjection(const DirectX::FXMMATRIX& view) noexcept(false);

            alignas(16)DirectX::XMFLOAT3 ProjectedPos;

            struct PointLightPixelCBuf
            {
                alignas(16)DirectX::XMFLOAT3 pos;
                alignas(16)DirectX::XMFLOAT3 ambient;
                alignas(16)DirectX::XMFLOAT3 diffuseColor;
                float diffuseIntensity;
                float attConst;
                float attLin;
                float attQuad;
            } PixelConstBufData;

        private:
            
            Transform * m_transform;
    };
}