#include "ParticleEmitterSystem.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer.h"

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
    ParticleEmitterSystem::ParticleEmitterSystem(registry_type* registry, graphics::Graphics* graphics)
        : m_emitterStates{},
          m_toAdd{},
          m_toRemove{},
          m_graphicsData{CreateParticleGraphicsData(graphics)},
          m_graphics{graphics}    
    {
        registry->RegisterOnAddCallback<ParticleEmitter>
        (
            [this](ParticleEmitter& emitter) { this->Add(emitter); }
        );

        registry->RegisterOnRemoveCallback<ParticleEmitter>
        (
            [this](Entity entity) { this->Remove(entity); }
        );
    }

    void ParticleEmitterSystem::UpdateParticles(float dt)
    {
        for(auto& state : m_emitterStates)
        {
            state.Update(dt);
        }
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

        for(auto entity : m_toRemove)
        {
            std::erase_if(m_emitterStates, [entity](auto& state)
            {
                return state.GetEntity() == entity;
            });
        }

        m_toRemove.clear();
    }

    void ParticleEmitterSystem::Emit(Entity entity, size_t count)
    {
        auto findPred = [entity](particle::EmitterState& state)
        {
            return state.GetEntity() == entity;
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

    void ParticleEmitterSystem::Add(ParticleEmitter&)
    {
        // m_graphics->GetRendererPtr()->RegisterParticleEmitter(&m_emitterStates);
        // m_toAdd.emplace_back(emitter.GetParentEntity(), emitter.GetInfo(), &m_graphicsData);
        // emitter.RegisterSystem(this);
    }

    void ParticleEmitterSystem::Remove(Entity entity)
    {
        m_toRemove.push_back(entity);
    }

    void ParticleEmitterSystem::Clear()
    {
        m_emitterStates.clear();
        m_emitterStates.shrink_to_fit();
        m_toAdd.clear();
        m_toAdd.shrink_to_fit();
        m_toRemove.clear();
        m_toRemove.shrink_to_fit();
        // m_graphics->GetRendererPtr()->ClearParticleEmitters();
    }
} // namespace nc::ecs