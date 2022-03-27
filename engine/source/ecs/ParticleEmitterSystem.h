#pragma once

#include "ecs/Registry.h"
#include "ecs/component/ParticleEmitter.h"
#include "particle/EmitterState.h"
#include "math/Random.h"
#include "ecs/component/Camera.h"

namespace nc::ecs
{
    class ParticleEmitterSystem
    {
        public:
            ParticleEmitterSystem(Registry* registry, float* dt, std::function<nc::Camera* ()> getCamera);

            /** Run is able to be run from the JobSystem, but it must finish before
             *  RenderParticles is called. ProcessFrameEvents should be called after rendering to
             *  finalize requests from game logic (additions/deletions). A side effect of this is
             *  particles won't be rendered until the frame after they are created. */
            void Run();
            std::span<const particle::EmitterState> GetParticles() const;
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
            float* m_dt;
            Random m_random;
            std::function<nc::Camera* ()> m_getCamera;
            Registry* m_registry;
    };
} // namespace nc::ecs