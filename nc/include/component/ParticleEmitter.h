#pragma once

#include "Component.h"
#include "math/Vector.h"
#include "math/Quaternion.h"
#include "utility/Range.h"
#include "random/Random.h"

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

        Range<Vector3> position = Vector3::Zero();
        Range<float> rotation = 0.0f;
        Range<float> scale = 1.0f;
        std::string particleTexturePath = "";
    };

    struct ParticleKinematicInfo
    {
        Range<Vector3> velocity = Vector3::Zero();
        float velocityOverTimeFactor = 0.5f;
        Range<float> rotation = 0.0f;
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
    struct StoragePolicy<ParticleEmitter>
    {
        using allow_trivial_destruction = std::true_type;
        using sort_dense_storage_by_address = std::true_type;
        using requires_on_add_callback = std::true_type;
        using requires_on_remove_callback = std::true_type;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<ParticleEmitter>(ParticleEmitter*);
    #endif
} // namespace nc