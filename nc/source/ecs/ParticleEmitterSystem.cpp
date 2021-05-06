#include "ParticleEmitterSystem.h"

namespace nc::ecs
{
    ParticleEmitterSystem::ParticleEmitterSystem(unsigned maxCount)
        : m_componentSystem{maxCount},
          m_emitterStates{},
          m_renderer{}
    {
    }

    void ParticleEmitterSystem::UpdateParticles(float dt)
    {
        for(auto& state : m_emitterStates)
        {
            state.Update(dt);
        }
    }

    void ParticleEmitterSystem::RenderParticles()
    {
        m_renderer.Render(m_emitterStates);
    }

    ParticleEmitter* ParticleEmitterSystem::Add(EntityHandle handle, ParticleInfo info)
    {
        auto& state = m_emitterStates.emplace_back(handle, info);
        return m_componentSystem.Add(handle, &state);
    }

    bool ParticleEmitterSystem::Remove(EntityHandle handle)
    {
        auto result = m_componentSystem.Remove(handle);
        
        if(result)
        {
            m_emitterStates.erase
            (
                std::remove_if(m_emitterStates.begin(), m_emitterStates.end(), [handle](auto& state)
                {
                    return state.GetHandle() == handle;
                }),
                m_emitterStates.end()
            );
        }

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
        m_emitterStates.clear();
        m_emitterStates.shrink_to_fit();
    }
} // namespace nc::ecs