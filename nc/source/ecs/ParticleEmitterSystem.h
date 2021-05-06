#pragma once

#include "component/ParticleEmitter.h"
#include "particle/ParticleRenderer.h"
#include "particle/EmitterState.h"

#include <memory>
#include <random>

namespace nc::ecs
{
    class ParticleEmitterSystem
    {
        public:
            ParticleEmitterSystem(unsigned maxCount);

            void UpdateParticles(float dt);
            void RenderParticles();

            // ComponentSystem Methods
            ParticleEmitter* Add(EntityHandle handle, ParticleInfo info);
            bool Remove(EntityHandle handle);
            bool Contains(EntityHandle handle) const;
            ParticleEmitter* GetPointerTo(EntityHandle handle);
            void Clear();

        private:
            ecs::ComponentSystem<ParticleEmitter> m_componentSystem;
            ecs::ComponentSystem<particle::EmitterState> m_stateComponentSystem;
            particle::ParticleRenderer m_renderer;
    };
} // namespace nc::ecs