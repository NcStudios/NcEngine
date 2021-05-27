#pragma once

#include "Component.h"
#include "math/Vector3.h"
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
        Vector3 position = Vector3::Zero();
        Vector3 positionRange = Vector3::Zero();
        float rotation = 0.0f;
        float rotationRange = 0.0f;
        float scale = 1.0f;
        float scaleRange = 0.0f;
        std::string particleTexturePath = "";
    };

    struct ParticleKinematicInfo
    {
        Vector3 velocity = Vector3::Zero();
        Vector3 velocityRange = Vector3::Zero();
        float velocityOverTimeFactor = 0.5f;
        float rotation = 0.0f;
        float rotationRange = 0.0f;
        float rotationOverTimeFactor = 0.5f;
        float scaleOverTimeFactor = 0.0f;
    };

    struct ParticleInfo
    {
        ParticleEmissionInfo emission;
        ParticleInitInfo init;
        ParticleKinematicInfo kinematic;
    };

    class ParticleEmitter : public Component
    {
        public:
            ParticleEmitter(EntityHandle handle, ParticleInfo info, ecs::ParticleEmitterSystem* emitterSystem);

            const ParticleInfo& GetInfo() const noexcept;
            void Emit(size_t count);
        
            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif

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
} // namespace nc