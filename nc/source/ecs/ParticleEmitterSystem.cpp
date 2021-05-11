#include "ParticleEmitterSystem.h"
#include "graphics/Graphics.h"

#include <algorithm>

namespace
{
    auto CreateParticleGraphicsData(nc::graphics::Graphics* graphics)
    {
        return nc::particle::GraphicsData
        {
            graphics->GetViewMatrix(),
            graphics->GetProjectionMatrix(),
            graphics
        };
    }
}

namespace nc::ecs
{
    ParticleEmitterSystem::ParticleEmitterSystem(unsigned maxCount, graphics::Graphics* graphics)
        : m_componentSystem{maxCount},
          m_emitterStates{},
          m_toAdd{},
          m_toRemove{},
          m_graphicsData{CreateParticleGraphicsData(graphics)},
          m_renderer{graphics}
    {
    }

    ecs::ComponentSystem<ParticleEmitter>* ParticleEmitterSystem::GetComponentSystem()
    {
        return &m_componentSystem;
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

    void ParticleEmitterSystem::ProcessFrameEvents()
    {
        // Could use linear allocator for add/remove vectors 
        m_emitterStates.insert
        (
            m_emitterStates.cend(),
            std::make_move_iterator(m_toAdd.begin()),
            std::make_move_iterator(m_toAdd.end())
        );

        m_toAdd.clear();

        for(auto handle : m_toRemove)
        {
            std::erase_if(m_emitterStates, [handle](auto& state)
            {
                return state.GetHandle() == handle;
            });
        }

        m_toRemove.clear();
    }

    void ParticleEmitterSystem::Emit(EntityHandle handle, size_t count)
    {
        auto findPred = [handle](particle::EmitterState& state)
        {
            return state.GetHandle() == handle;
        };

        auto pos = std::ranges::find_if(m_emitterStates, findPred);

        if(pos == m_emitterStates.end())
        {
            pos = std::ranges::find_if(m_toAdd, findPred);
            if(pos == m_toAdd.end())
                throw std::runtime_error("ParticleEmitterSystem::Emit - Particle emitter does not exist");
        }

        pos->Emit(count);
    }

    ParticleEmitter* ParticleEmitterSystem::Add(EntityHandle handle, ParticleInfo info)
    {
        m_toAdd.emplace_back(handle, info, &m_graphicsData);
        return m_componentSystem.Add(handle, this);
    }

    bool ParticleEmitterSystem::Remove(EntityHandle handle)
    {
        auto result = m_componentSystem.Remove(handle);
        if(result)
            m_toRemove.push_back(handle);

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
        m_toAdd.clear();
        m_toAdd.shrink_to_fit();
        m_toRemove.clear();
        m_toRemove.shrink_to_fit();
    }
} // namespace nc::ecs