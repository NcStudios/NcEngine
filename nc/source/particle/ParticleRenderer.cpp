#include "ParticleRenderer.h"
#include "EmitterState.h"
#include "graphics/Graphics.h"
#include "graphics/techniques/ParticleTechnique.h"
#include "graphics/techniques/TechniqueManager.h"

namespace nc::particle
{
    ParticleRenderer::ParticleRenderer(graphics::Graphics* graphics)
        : m_graphics{graphics},
          m_pixelCBuff{std::make_unique<graphics::d3dresource::PixelConstantBuffer<graphics::MvpMatrices>>(2u)},
          m_vertexCBuff{std::make_unique<graphics::d3dresource::VertexConstantBuffer<graphics::MvpMatrices>>(0u)}
    {
        graphics::TechniqueManager::GetTechnique<graphics::ParticleTechnique>();
    }

    void ParticleRenderer::Render(const std::vector<EmitterState>& emitterStates)
    {
        graphics::ParticleTechnique::BindCommonResources();
        constexpr auto indexCount = graphics::ParticleTechnique::ParticleMeshIndexCount;

        for(const auto& emitterState : emitterStates)
        {
            auto [index, matrices] = emitterState.GetSoA()->View<EmitterState::MvpMatricesIndex>();

            for(; index.Valid(); ++index)
            {
                BindMatrices(matrices[index]);
                m_graphics->DrawIndexed(indexCount);
            }
        }
    }

    void ParticleRenderer::BindMatrices(const graphics::MvpMatrices& matrices)
    {
        m_vertexCBuff->Update(matrices);
        m_vertexCBuff->Bind();
        m_pixelCBuff->Update(matrices);
        m_pixelCBuff->Bind();
    }
} // namespace nc::particle