#pragma once
#include "Graphics.h"
#include "ConstantBuffer.h"

namespace nc::graphics
{
    class PointLight
    {
        public:
            PointLight(internal::Graphics& graphics, float radius = 0.5f);
            
            void SpawnControlWindow() noexcept;

            void Draw(internal::Graphics& graphics) const noexcept;
            void Bind(internal::Graphics& graphics) const noexcept;

        private:
            struct PointLightCBuf
            {
                alignas(16)DirectX::XMFLOAT3 pos;
                alignas(16)DirectX::XMFLOAT3 materialColor;
                alignas(16)DirectX::XMFLOAT3 ambient;
                alignas(16)DirectX::XMFLOAT3 diffuseColor;
                float diffuseIntensity;
                float attConst;
                float attLin;
                float attQuad;
            } m_constBufData;

            //DirectX::XMFLOAT3 m_pos = { 0.0f, 0.0f, 0.0f };
            mutable internal::PixelConstantBuffer<PointLightCBuf> m_cBuf;
    };
}