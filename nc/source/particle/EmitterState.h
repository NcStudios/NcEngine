#pragma once

#include "component/ParticleEmitter.h"
#include "ecs/SoA.h"

#include <vector>

namespace nc::particle
{
    struct Particle
    {
        float maxLifetime;
        float currentLifetime;
        Vector3 position;
        Quaternion rotation;
        Vector3 scale;
    };

    /** @todo Maybe the SoA should exist in ParticleEmitterSystem and contain
     *  all of the particles. Rendering would be tricky with different ranges
     *  requiring different textures. */

    class EmitterState
    {
        public:
            using ParticleSoA = ecs::SoA<float, float, Vector3, Quaternion, Vector3>;
            static constexpr size_t MaxLifetimesIndex = 0u;
            static constexpr size_t CurrentLifetimesIndex = 1u;
            static constexpr size_t PositionsIndex = 2u;
            static constexpr size_t RotationsIndex = 3u;
            static constexpr size_t ScalesIndex = 4u;

            EmitterState(EntityHandle handle, const ParticleInfo& info);

            void Emit(size_t count);
            void Update(float dt);
            const ParticleSoA* GetSoA() const;
            EntityHandle GetHandle() const;

        private:
            ParticleInfo m_info;
            ParticleSoA m_soa;
            EntityHandle m_handle;
    };
}