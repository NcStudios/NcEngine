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
class ParticleSubsystem;

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
    float velocityOverTimeFactor = 0.0f;
    float rotationMin = 0.0f;
    float rotationMax = 0.0f;
    float rotationOverTimeFactor = 0.0f;
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
        // need move ops?
        ParticleEmitter(Entity entity, ParticleInfo info);
        ~ParticleEmitter() noexcept;

        // prob not ok
        ParticleEmitter(ParticleEmitter&&) = default;
        ParticleEmitter& operator=(ParticleEmitter&&) = default;

        auto GetInfo() const noexcept -> const ParticleInfo& { return m_info; }
        void SetInfo(const ParticleInfo& info);
        void Emit(size_t count);

        // todo: delete
        void RegisterSystem(ParticleEmitterSystem*) {}

        /** @cond internal */
        static void RegisterSubsystem(ParticleSubsystem* subsystem)
        {
            s_subsystem = subsystem;
        }
        /** @endcond internal */

    private:
        static inline ParticleSubsystem* s_subsystem = nullptr;

        ParticleInfo m_info;
};
} // namespace nc::graphics

// todo: delete
namespace nc
{
template<>
struct StoragePolicy<graphics::ParticleEmitter> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = true;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc
