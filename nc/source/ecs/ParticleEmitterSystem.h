#pragma once

#include "ComponentSystem.h"
#include "component/ParticleEmitter.h"
#include "particle/ParticleRenderer.h"
#include "particle/EmitterState.h"

namespace nc::graphics { class Graphics; }

namespace nc::ecs
{
    class ParticleEmitterSystem
    {
        public:
            ParticleEmitterSystem(unsigned maxCount, graphics::Graphics* graphics);

            ecs::ComponentSystem<ParticleEmitter>* GetComponentSystem();

            /** UpdateParticles is able to be run from the JobSystem, but it must finish before
             *  RenderParticles is called. ProcessFrameEvents should be called after rendering to
             *  finalize requests from game logic (additions/deletions). A side effect of this is
             *  particles won't be rendered until the frame after they are created. */
            void UpdateParticles(float dt);
            void RenderParticles();
            void ProcessFrameEvents();

            // this may need to be delayed too
            void Emit(EntityHandle handle, size_t count);

            // ComponentSystem Methods
            ParticleEmitter* Add(EntityHandle handle, ParticleInfo info);
            bool Remove(EntityHandle handle);
            bool Contains(EntityHandle handle) const;
            ParticleEmitter* GetPointerTo(EntityHandle handle);
            void Clear();

        private:
            ecs::ComponentSystem<ParticleEmitter> m_componentSystem;
            std::vector<particle::EmitterState> m_emitterStates;
            std::vector<particle::EmitterState> m_toAdd;
            std::vector<EntityHandle> m_toRemove;
            particle::GraphicsData m_graphicsData;
            particle::ParticleRenderer m_renderer;

    };
} // namespace nc::ecs