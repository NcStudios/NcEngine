#include "ParticleEmitterSystem.h"
#include "ecs/Transform.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer.h"

#include "optick/optick.h"
#include <algorithm>

namespace nc::ecs
{
    ParticleEmitterSystem::ParticleEmitterSystem(Registry* registry, float* dt, std::function<nc::graphics::Camera* ()> getCamera)
        : m_emitterStates{},
          m_toAdd{},
          m_toRemove{},
          m_dt{ dt },
          m_random{ Random() },
          m_getCamera{ getCamera },
          m_registry{ registry },
          m_onAddConnection{ registry->OnAdd<graphics::ParticleEmitter>().Connect(this, &ParticleEmitterSystem::Add) },
          m_onRemoveConnection{ registry->OnRemove<graphics::ParticleEmitter>().Connect(this, &ParticleEmitterSystem::Remove)}
    {
    }

    void ParticleEmitterSystem::Run()
    {
        OPTICK_CATEGORY("ParticleModule", Optick::Category::VFX);
        const float dt = *m_dt;
        const auto* camera = m_getCamera();
        const auto* transform = m_registry->Get<Transform>(camera->ParentEntity());
        const auto camRotation = transform->Rotation();
        const auto camForward = transform->Forward();

        for (auto& state : m_emitterStates)
        {
            state.Update(dt, camRotation, camForward);
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

        for (auto entity : m_toRemove)
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

        if (pos == m_emitterStates.end())
        {
            pos = std::ranges::find_if(m_toAdd, findPred);
            if (pos == m_toAdd.end())
                throw NcError("Particle emitter does not exist");
        }

        pos->Emit(count);
    }

    std::span<const particle::EmitterState> ParticleEmitterSystem::GetParticles() const
    {
        return m_emitterStates;
    }

    void ParticleEmitterSystem::Add(graphics::ParticleEmitter& emitter)
    {
        m_toAdd.emplace_back(emitter.ParentEntity(), emitter.GetInfo(), &m_random);
        emitter.RegisterSystem(this);
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
