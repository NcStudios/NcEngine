#include "ParticleSystemManager.h"


namespace
{
    using namespace nc;

    Particle CreateParticle(const ParticleInfo& info)
    {
        Vector3 position = info.particlePosition * (rand() % 5);
        float randRot = rand() % 3;
        Quaternion rotation = Quaternion::FromEulerAngles(randRot, randRot, randRot);
        Vector3 scale = Vector3::Splat((rand() % 3) + 1);

        return Particle
        {
            .maxLifetime = info.particleLifetime,
            .currentLifetime = 0.0f,
            .position = position,
            .rotation = rotation,
            .scale = scale
        };
    }

    DirectX::XMMATRIX ComposeMatrix(const Vector3& scale, const Quaternion& r, const Vector3& pos)
    {
        return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * 
               DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(r.x, r.y, r.z, r.w)) *
               DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
    }
}

namespace nc::ecs
{
    /** ParticleSystemData */
    ParticleSystemData::ParticleSystemData(EntityHandle handle, const ParticleInfo& info)
        : Component{handle},
          m_info{info},
          m_particles{},
          m_pixelCBuff{std::make_unique<graphics::d3dresource::PixelConstantBuffer<Transforms>>(2u)},
          m_vertexCBuff{std::make_unique<graphics::d3dresource::VertexConstantBuffer<Transforms>>(0u)},
          m_viewMatrix{graphics::d3dresource::GraphicsResourceManager::GetGraphics()->GetViewMatrix()},
          m_projectionMatrix{graphics::d3dresource::GraphicsResourceManager::GetGraphics()->GetProjectionMatrix()}
    {
        graphics::TechniqueManager::GetTechnique<graphics::ParticleTechnique>();
        Emit(m_info.initialEmissionCount);
    }

    void ParticleSystemData::Emit(size_t count)
    {
        for(unsigned i = 0; i < count; ++i)
            m_particles.push_back(CreateParticle(m_info));
    }

    void ParticleSystemData::Update(float dt)
    {
        std::vector<unsigned> toRemove;

        for(unsigned i = 0u; i < m_particles.size(); ++i)
        {
            auto& particle = m_particles[i];
            particle.currentLifetime += dt;
            if(particle.currentLifetime >= particle.maxLifetime)
                toRemove.push_back(i);

            for(auto i : toRemove)
            {
                m_particles[i] = m_particles.back();
                m_particles.pop_back();
            }
        }
    }

    void ParticleSystemData::BindTransformationMatrix(DirectX::FXMMATRIX matrix)
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

    void ParticleSystemData::RenderUpdate()
    {
        graphics::ParticleTechnique::BindCommonResources();
        auto* graphics = graphics::d3dresource::GraphicsResourceManager::GetGraphics();
        auto indexCount = 6u;

        for(const auto& particle : m_particles)
        {
            auto matrix = ComposeMatrix(particle.scale, particle.rotation, particle.position);
            BindTransformationMatrix(matrix);
            graphics->DrawIndexed(indexCount);
        }
    }

    /** ParticleSystemManager */
    ParticleSystemManager::ParticleSystemManager(unsigned maxCount)
        : m_componentSystem{maxCount},
          m_dataComponentSystem{maxCount}
    {
    }

    void ParticleSystemManager::UpdateParticles(float dt)
    {
        for(auto& particleSystemData : m_dataComponentSystem.GetComponents())
        {
            particleSystemData->Update(dt);
        }
    }

    void ParticleSystemManager::RenderParticles()
    {
        for(auto& particleSystemData : m_dataComponentSystem.GetComponents())
            particleSystemData->RenderUpdate();
    }

    ParticleSystem* ParticleSystemManager::Add(EntityHandle handle, ParticleInfo info)
    {
        auto* data = m_dataComponentSystem.Add(handle, info);
        return m_componentSystem.Add(handle, data);
    }

    bool ParticleSystemManager::Remove(EntityHandle handle)
    {
        auto result = m_componentSystem.Remove(handle);
        if(result)
            m_dataComponentSystem.Remove(handle);
        
        return result;
    }

    bool ParticleSystemManager::Contains(EntityHandle handle) const
    {
        return m_componentSystem.Contains(handle);
    }

    ParticleSystem* ParticleSystemManager::GetPointerTo(EntityHandle handle)
    {
        return m_componentSystem.GetPointerTo(handle);
    }

    // auto ParticleSystemManager::GetComponents() -> ecs::ComponentSystem<ParticleSystem>::ContainerType&
    // {

    // }

    void ParticleSystemManager::Clear()
    {
        m_componentSystem.Clear();
        m_dataComponentSystem.Clear();
    }
} // namespace nc::ecs