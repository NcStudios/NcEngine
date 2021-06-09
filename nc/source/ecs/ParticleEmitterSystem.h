#pragma once

#include "component/ParticleEmitter.h"
#include "particle/ParticleRenderer.h"
#include "particle/EmitterState.h"

namespace nc::graphics { class Graphics; }

namespace nc::ecs
{
    class ParticleEmitterSystem
    {
        public:

        #ifdef USE_VULKAN
            ParticleEmitterSystem();
        #else
            ParticleEmitterSystem(graphics::Graphics* graphics);
        #endif

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
            void Add(const ParticleEmitter& emitter);
            void Remove(EntityHandle handle);
            void Clear();

        private:
            std::vector<particle::EmitterState> m_emitterStates;
            std::vector<particle::EmitterState> m_toAdd;
            std::vector<EntityHandle> m_toRemove;
        #ifndef USE_VULKAN
            particle::GraphicsData m_graphicsData;
            particle::ParticleRenderer m_renderer;
        #endif

    };
} // namespace nc::ecs