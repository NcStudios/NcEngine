#include "ParticleRenderer.h"
#include "EmitterState.h"
#include "graphics/Graphics.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/techniques/ParticleTechnique.h"
#include "graphics/techniques/TechniqueManager.h"

namespace
{
    using namespace nc;

    DirectX::XMMATRIX ComposeMatrix(const Vector3& scale, const Quaternion& r, const Vector3& pos)
    {
        return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * 
               DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(r.x, r.y, r.z, r.w)) *
               DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
    }
}

namespace nc::particle
{
    ParticleRenderer::ParticleRenderer()
        : m_pixelCBuff{std::make_unique<graphics::d3dresource::PixelConstantBuffer<Transforms>>(2u)},
          m_vertexCBuff{std::make_unique<graphics::d3dresource::VertexConstantBuffer<Transforms>>(0u)},
          m_viewMatrix{graphics::d3dresource::GraphicsResourceManager::GetGraphics()->GetViewMatrix()},
          m_projectionMatrix{graphics::d3dresource::GraphicsResourceManager::GetGraphics()->GetProjectionMatrix()}
    {
        graphics::TechniqueManager::GetTechnique<graphics::ParticleTechnique>();
    }

    void ParticleRenderer::BindMatrix(DirectX::FXMMATRIX matrix)
    {
        const auto modelView = matrix * m_viewMatrix;
        Transforms t
        {
            DirectX::XMMatrixTranspose(modelView),
            DirectX::XMMatrixTranspose(modelView * m_projectionMatrix)
        };

        m_vertexCBuff->Update(t);
        m_vertexCBuff->Bind();
        m_pixelCBuff->Update(t);
        m_pixelCBuff->Bind();
    }

    void ParticleRenderer::Render(const std::vector<EmitterState>& emitterStates)
    {
        graphics::ParticleTechnique::BindCommonResources();

        /** @todo don't fetch gfx like this and figure out how to get index count */
        auto* graphics = graphics::d3dresource::GraphicsResourceManager::GetGraphics();
        auto indexCount = 6u;

        for(auto& emitterState : emitterStates)
        {
            constexpr auto pi = EmitterState::PositionsIndex;
            constexpr auto ri = EmitterState::RotationsIndex;
            constexpr auto si = EmitterState::ScalesIndex;
            auto [index, positions, rotations, scales] = emitterState.GetSoA()->View<pi, ri, si>();
            
            while(index.Valid())
            {
                auto matrix = ComposeMatrix(scales[index], rotations[index], positions[index]);
                BindMatrix(matrix);
                graphics->DrawIndexed(indexCount);
                ++index;
            }
        }
    }
} // namespace nc::particle