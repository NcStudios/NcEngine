#include "ParticleEmitterSystem.h"

namespace nc::ecs
{
    ParticleEmitterSystem::ParticleEmitterSystem(unsigned maxCount)
        : m_componentSystem{maxCount},
          m_stateComponentSystem{maxCount},
          m_renderer{}
    {
    }

    void ParticleEmitterSystem::UpdateParticles(float dt)
    {
        for(auto& emitterState : m_stateComponentSystem.GetComponents())
        {
            emitterState->Update(dt);
        }
    }

    void ParticleEmitterSystem::RenderParticles()
    {
        m_renderer.Render(m_stateComponentSystem.GetComponents());
    }

    ParticleEmitter* ParticleEmitterSystem::Add(EntityHandle handle, ParticleInfo info)
    {
        auto* emitterState = m_stateComponentSystem.Add(handle, info);
        return m_componentSystem.Add(handle, emitterState);
    }

    bool ParticleEmitterSystem::Remove(EntityHandle handle)
    {
        auto result = m_componentSystem.Remove(handle);
        if(result)
            m_stateComponentSystem.Remove(handle);
        
        return result;
    }

    bool ParticleEmitterSystem::Contains(EntityHandle handle) const
    {
        return m_componentSystem.Contains(handle);
    }

    ParticleEmitter* ParticleEmitterSystem::GetPointerTo(EntityHandle handle)
    {
        return m_componentSystem.GetPointerTo(handle);
    }

    void ParticleEmitterSystem::Clear()
    {
        m_componentSystem.Clear();
        m_stateComponentSystem.Clear();
    }
} // namespace nc::ecs