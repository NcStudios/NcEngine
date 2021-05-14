#pragma once

#include "graphics/MvpMatrices.h"
#include "graphics/d3dresource/ConstantBufferResources.h"

namespace nc::particle
{
    class EmitterState;

    class ParticleRenderer
    {
        public:
            ParticleRenderer(graphics::Graphics* graphics);

            void Render(const std::vector<EmitterState>& emitterStates);

        private:
            void BindMatrices(const graphics::MvpMatrices& matrices);

            graphics::Graphics* m_graphics;
            std::unique_ptr<graphics::d3dresource::PixelConstantBuffer<graphics::MvpMatrices>> m_pixelCBuff;
            std::unique_ptr<graphics::d3dresource::VertexConstantBuffer<graphics::MvpMatrices>> m_vertexCBuff;
    };
}