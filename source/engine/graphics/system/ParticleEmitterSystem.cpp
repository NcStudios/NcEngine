#include "ParticleEmitterSystem.h"
#include "ecs/Transform.h"
#include "time/Time.h"

#include "optick.h"

#include <algorithm>

namespace nc::graphics
{
    ParticleEmitterSystem::ParticleEmitterSystem(Registry* registry, std::function<nc::graphics::Camera* ()> getCamera)
        : m_emitterStates{},
          m_toAdd{},
          m_toRemove{},
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
        const float dt = time::DeltaTime();
        const auto [camRotation, camForward] = [this]()
        {
            if (auto camera = m_getCamera())
            {
                const auto* transform = m_registry->Get<Transform>(camera->ParentEntity());
                return std::make_pair(transform->RotationXM(), transform->ForwardXM());
            }

            return std::make_pair(DirectX::XMQuaternionIdentity(), DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
        }();

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

    void ParticleEmitterSystem::UpdateInfo(graphics::ParticleEmitter& emitter)
    {
        auto findPred = [entity = emitter.ParentEntity()](particle::EmitterState& state)
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

        pos->UpdateInfo(emitter.GetInfo());
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

    void ParticleEmitterSystem::Update(graphics::ParticleEmitter& emitter)
    {
        auto findPred = [entity = emitter.ParentEntity()](particle::EmitterState& state)
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

        pos->UpdateInfo(emitter.GetInfo());
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
} // namespace nc::graphics
