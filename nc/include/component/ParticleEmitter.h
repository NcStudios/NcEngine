#pragma once

#include "Component.h"
#include "math/Vector3.h"
#include "math/Quaternion.h"

#include <string>

namespace nc
{
    struct ParticleInfo
    {
        unsigned maxParticleCount = 100u;
        unsigned initialEmissionCount = 0u;
        unsigned periodicEmissionCount = 0u;
        float periodicEmissionFrequency = 0.0f;

        float particleLifetime = 5.0f;
        Vector3 particlePosition = Vector3::Zero();
        Quaternion particleRotation = Quaternion::Identity();
        Vector3 particleScale = Vector3::One();

        std::string particleTexturePath = "";
    };

    namespace ecs { class ParticleEmitterSystem; }

    class ParticleEmitter : public Component
    {
        public:
            ParticleEmitter(EntityHandle handle, ecs::ParticleEmitterSystem* emitterSystem);

            void Emit(size_t count);
        
        private:
            ecs::ParticleEmitterSystem* m_emitterSystem;
    };
} // namespace nc