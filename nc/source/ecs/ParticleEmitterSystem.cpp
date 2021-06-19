#include "ParticleEmitterSystem.h"
#include "graphics/Graphics.h"

#include <algorithm>

namespace
{
    #ifndef USE_VULKAN
    auto CreateParticleGraphicsData(nc::graphics::Graphics* graphics)
    {
        return nc::particle::GraphicsData
        {
            graphics->GetViewMatrix(),
            graphics->GetProjectionMatrix(),
            graphics
        };
    }
    #endif
}

namespace nc::ecs
{
    #ifdef USE_VULKAN
    ParticleEmitterSystem::ParticleEmitterSystem(registry_type* registry)
    #else
    ParticleEmitterSystem::ParticleEmitterSystem(registry_type* registry, graphics::Graphics* graphics)
    #endif
        : m_emitterStates{},
          m_toAdd{}
          #ifndef USE_VULKAN
          ,
          m_toRemove{},
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
        #ifndef USE_VULKAN
        m_renderer.Render(m_emitterStates);
        #endif
    }

    void ParticleEmitterSystem::ProcessFrameEvents()
    {
        #ifndef USE_VULKAN
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
        #endif
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

    void ParticleEmitterSystem::Add([[maybe_unused]] ParticleEmitter& emitter)
    {
        #ifndef USE_VULKAN
        m_toAdd.emplace_back(emitter.GetParentEntity(), emitter.GetInfo(), &m_graphicsData);
        emitter.RegisterSystem(this);
        #endif
    }

    void ParticleEmitterSystem::Remove(Entity entity)
    {
        #ifndef USE_VULKAN
        m_toRemove.push_back(entity);
        #endif
    }

    void ParticleEmitterSystem::Clear()
    {
        #ifndef USE_VULKAN
        m_emitterStates.clear();
        m_emitterStates.shrink_to_fit();
        m_toAdd.clear();
        m_toAdd.shrink_to_fit();
        m_toRemove.clear();
        m_toRemove.shrink_to_fit();
        #endif
    }
} // namespace nc::ecs