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
    ParticleEmitterSystem::ParticleEmitterSystem(graphics::Graphics* graphics)
        : m_emitterStates{},
          m_toAdd{},
          m_toRemove{},
          m_graphicsData{CreateParticleGraphicsData(graphics)},
          m_renderer{graphics}
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

    void ParticleEmitterSystem::Add(const ParticleEmitter& emitter)
    {
        m_toAdd.emplace_back(emitter.GetParentHandle(), emitter.GetInfo(), &m_graphicsData);
    }

    void ParticleEmitterSystem::Remove(EntityHandle handle)
    {
        m_toRemove.push_back(handle);
    }

    void ParticleEmitterSystem::Clear()
    {
        m_emitterStates.clear();
        m_emitterStates.shrink_to_fit();
        m_toAdd.clear();
        m_toAdd.shrink_to_fit();
        m_toRemove.clear();
        m_toRemove.shrink_to_fit();
    }
} // namespace nc::ecs