#pragma once
#include "Component.h"
#include "directx/math/DirectXMath.h"

#include "graphics/d3dresource/GraphicsResource.h"
#include "config/Config.h"

namespace nc::graphics { class Graphics; }

namespace nc
{
    class Transform;

    class PointLight final: public Component
    {
        public:
            PointLight(ComponentHandle handle, EntityHandle parentHandle) noexcept;
            PointLight() = default;
            ~PointLight() = default;
            PointLight(PointLight&&) = default;
            PointLight& operator=(PointLight&&) = default;
            PointLight(const PointLight&) = delete;
            PointLight& operator=(const PointLight&) = delete;

            void Set(DirectX::XMFLOAT3 pos,  float radius = 0.5f, DirectX::XMFLOAT3 ambient = {0.65f, 0.65f, 0.65f}, DirectX::XMFLOAT3 diffuseColor = {1.0f, 1.0f, 1.0f}, float diffuseIntensity = 0.9f, float attConst = 2.61f, float attLin = 0.1819f, float attQuad =  0.0000001f);

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