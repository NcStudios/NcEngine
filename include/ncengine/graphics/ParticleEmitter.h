/**
 * @file ParticleEmitter.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/ecs/Component.h"

#include "ncmath/Vector.h"
#include "ncmath/Quaternion.h"

#include <string>

namespace nc::graphics
{
class ParticleEmitterSystem;

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
    float scaleMin = 1.0f;
    float scaleMax = 1.0f;
    std::string particleTexturePath = asset::DefaultParticle;
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
        void SetInfo(ParticleInfo info);
        void Emit(size_t count);

        void RegisterSystem(ParticleEmitterSystem* system);

    private:
        ParticleInfo m_info;
        ParticleEmitterSystem* m_emitterSystem;
};
} // namespace nc::graphics

namespace nc
{
template<>
struct StoragePolicy<graphics::ParticleEmitter> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = true;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc
