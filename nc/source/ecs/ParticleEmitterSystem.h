#pragma once

#include "ecs/Registry.h"
#include "ecs/component/ParticleEmitter.h"
#include "particle/EmitterState.h"

namespace nc::ecs
{
    /** @todo I skipped making this a module because Jare was working
     *  on fixing this at the same time:
     *    - Derive from Module
     *    - Rename ParticleModule (there is no api interface so no need for impl suffix)
     *    - Mark Clear as override
     *    - Implement GetWorkload
     *        - job1 calls Run + HookPoint::Free
     *        - job2 calls ProcessFrameEvents + HookPoint::PostFrameSync
     *    - Update in Runtime.cpp
     *    - Update in Modules.h */
    class ParticleEmitterSystem
    {
        public:
            ParticleEmitterSystem(Registry* registry, float* dt);

            /** Run is able to be run from the JobSystem, but it must finish before
             *  RenderParticles is called. ProcessFrameEvents should be called after rendering to
             *  finalize requests from game logic (additions/deletions). A side effect of this is
             *  particles won't be rendered until the frame after they are created. */
            void Run();
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
            float* m_dt;
    };
} // namespace nc::ecs