#pragma once

#include "Component.h"
#include "math/Vector.h"
#include "math/Quaternion.h"

#include <string>

namespace nc
{
    namespace ecs { class ParticleEmitterSystem; }

    struct ParticleEmissionInfo
    {
        unsigned maxParticleCount = 100u;
        unsigned initialEmissionCount = 0u;
        unsigned periodicEmissionCount = 0u;
        float periodicEmissionFrequency = 0.0f;
    };

    struct ParticleInitInfo
    {
        float lifetime = 5.0f;
        Vector3 positionMin = Vector3::Zero();
        Vector3 positionMax = Vector3::Zero();
        float rotationMin = 0.0f;
        float rotationMax = 0.0f;
        float scaleMin = 0.0f;
        float scaleMax = 0.0f;
        std::string particleTexturePath = "";
    };

    struct ParticleKinematicInfo
    {
        Vector3 velocityMin = Vector3::Zero();
        Vector3 velocityMax = Vector3::Zero();
        float velocityOverTimeFactor = 0.5f;
        float rotationMin = 0.0f;
        float rotationMax = 0.0f;
        float rotationOverTimeFactor = 0.5f;
        float scaleOverTimeFactor = 0.0f;
    };

    struct ParticleInfo
    {
        ParticleEmissionInfo emission;
        ParticleInitInfo init;
        ParticleKinematicInfo kinematic;
    };

    class ParticleEmitter final : public ComponentBase
    {
        NC_ENABLE_IN_EDITOR(ParticleEmitter)
        
        public:
            ParticleEmitter(Entity entity, ParticleInfo info);

            const ParticleInfo& GetInfo() const noexcept;
            void Emit(size_t count);
        
            void RegisterSystem(ecs::ParticleEmitterSystem* system);

        private:
            ParticleInfo m_info;
            ecs::ParticleEmitterSystem* m_emitterSystem;
    };

    template<>
    struct StoragePolicy<ParticleEmitter> : DefaultStoragePolicy
    {
        static constexpr bool enable_on_add_callbacks = true;
        static constexpr bool enable_on_remove_callbacks = true;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<ParticleEmitter>(ParticleEmitter*);
    #endif
} // namespace nc