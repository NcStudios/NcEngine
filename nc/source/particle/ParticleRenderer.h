#pragma once

#include "directx/math/DirectXMath.h"
#include "graphics/d3dresource/ConstantBufferResources.h"

namespace nc::particle
{
    class EmitterState;

    class ParticleRenderer
    {
        public:
            ParticleRenderer();

            void Render(const std::vector<EmitterState>& emitterStates);

        private:
            void BindMatrix(DirectX::FXMMATRIX matrix);

            struct Transforms
            {
                DirectX::XMMATRIX modelView;
                DirectX::XMMATRIX model;
            };

            std::unique_ptr<graphics::d3dresource::PixelConstantBuffer<Transforms>> m_pixelCBuff;
            std::unique_ptr<graphics::d3dresource::VertexConstantBuffer<Transforms>> m_vertexCBuff;
            DirectX::FXMMATRIX m_viewMatrix;
            DirectX::FXMMATRIX m_projectionMatrix;
    };
}