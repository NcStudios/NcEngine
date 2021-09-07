#pragma once

#include "Ecs.h"
#include "component/ParticleEmitter.h"
#include "particle/EmitterState.h"

namespace nc::graphics { class Graphics; }

namespace nc::ecs
{
    class ParticleEmitterSystem
    {
        public:
            ParticleEmitterSystem(registry_type* registry, graphics::Graphics* graphics);

            /** UpdateParticles is able to be run from the JobSystem, but it must finish before
             *  RenderParticles is called. ProcessFrameEvents should be called after rendering to
             *  finalize requests from game logic (additions/deletions). A side effect of this is
             *  particles won't be rendered until the frame after they are created. */
            void UpdateParticles(float dt);
            void RenderParticles();
            void ProcessFrameEvents();

            // this may need to be delayed too
            void Emit(Entity entity, size_t count);

            // ComponentSystem Methods
            void Add(ParticleEmitter& emitter);
            void Remove(Entity entity);
            void Clear();

        private:
            std::vector<particle::EmitterState> m_emitterStates;
            std::vector<particle::EmitterState> m_toAdd;
            std::vector<Entity> m_toRemove;

            particle::GraphicsData m_graphicsData;
            graphics::Graphics* m_graphics;

    };
} // namespace nc::ecs