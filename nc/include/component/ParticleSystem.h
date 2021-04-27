#pragma once

#include "Component.h"
#include "math/Vector3.h"
#include "math/Quaternion.h"

#include <string>

namespace nc
{
    struct ParticleInfo
    {
        unsigned initialEmissionCount = 0u;
        unsigned periodicEmissionCount = 0u;
        float periodicEmissionFrequency = 0.0f;

        float particleLifetime = 5.0f;
        Vector3 particlePosition = Vector3::Zero();
        Quaternion particleRotation = Quaternion::Identity();
        Vector3 particleScale = Vector3::One();

        std::string particleTexturePath = "";
    };

    struct Particle
    {
        float maxLifetime;
        float currentLifetime;
        Vector3 position;
        Quaternion rotation;
        Vector3 scale;
    };

    namespace ecs { class ParticleSystemData; }

    class ParticleSystem : public Component
    {
        public:
            ParticleSystem(EntityHandle handle, ecs::ParticleSystemData* data);

            void Emit(size_t count);
        
        private:
            ecs::ParticleSystemData* m_data;
    };
} // namespace nc