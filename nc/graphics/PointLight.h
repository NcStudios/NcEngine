#pragma once
//#include "Graphics.h"

#include "DirectXMath.h"

#include "ConstantBuffer.h"

namespace nc::graphics
{
    class Graphics;

    class PointLight
    {
        public:
            PointLight(Graphics& graphics, DirectX::XMFLOAT3 pos, float radius = 0.5f);
            
            void SpawnControlWindow() noexcept;

            //void Draw(internal::Graphics& graphics) const noexcept;
            void Bind(Graphics& graphics, DirectX::FXMMATRIX view) const noexcept;

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

            mutable d3dresource::PixelConstantBuffer<PointLightCBuf> m_cBuf;
    };
}