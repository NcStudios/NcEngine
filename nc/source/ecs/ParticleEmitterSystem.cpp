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
    #ifdef USE_VULKAN
    ParticleEmitterSystem::ParticleEmitterSystem(registry_type* registry)
    #else
    ParticleEmitterSystem::ParticleEmitterSystem(registry_type* registry, graphics::Graphics* graphics)
    #endif
        : m_emitterStates{},
          m_toAdd{},
          m_toRemove{},
          #ifdef USE_VULKAN
          m_graphicsData{nullptr},
          m_renderer{nullptr}
          #else
          m_graphicsData{CreateParticleGraphicsData(graphics)},
          m_renderer{graphics}
          #endif
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

    void ParticleEmitterSystem::Add(ParticleEmitter& emitter)
    {
        emitter.RegisterSystem(this);
        m_toAdd.emplace_back(emitter.GetParentEntity(), emitter.GetInfo(), &m_graphicsData);
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
    }
} // namespace nc::ecs