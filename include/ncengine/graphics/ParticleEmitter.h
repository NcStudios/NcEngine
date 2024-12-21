/**
 * @file ParticleEmitter.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncengine/ecs/Component.h"

#include "ncmath/Vector.h"
#include "ncmath/Quaternion.h"

#include <string>

namespace nc
{
namespace graphics
{
class ParticleSubsystem;
} // namespace graphics

/** @brief  */
struct ParticleEmissionInfo
{
    unsigned maxParticleCount = 100u;
    unsigned initialEmissionCount = 0u;
    unsigned periodicEmissionCount = 0u;
    float periodicEmissionFrequency = 0.0f;
};

/** @brief  */
struct ParticleInitInfo
{
    float lifetime = 5.0f;
    Vector3 positionMin = Vector3::Zero();
    Vector3 positionMax = Vector3::Zero();
    float rotationMin = 0.0f;
    float rotationMax = 0.0f;
    float scaleMin = 1.0f;
    float scaleMax = 1.0f;
};

/** @brief  */
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

/** @brief  */
struct ParticleInfo
{
    ParticleEmissionInfo emission;
    ParticleInitInfo init;
    ParticleKinematicInfo kinematic;
};

/** @brief  */
class ParticleEmitter
{
    public:
        ParticleEmitter(Entity entity,
                        const asset::TextureView& texture,
                        const ParticleInfo& info = {});

        ParticleEmitter(ParticleEmitter&& other) noexcept
            : m_self{std::exchange(other.m_self, Entity::Null())},
              m_info{other.m_info}
        {
        }

        ParticleEmitter& operator=(ParticleEmitter&& other) noexcept
        {
            if (this != &other)
            {
                Release();
                m_self = std::exchange(other.m_self, Entity::Null());
                m_info = std::move(other.m_info);
            }

            return *this;
        }

        ~ParticleEmitter() noexcept
        {
            Release();
        }

        /** @name General Functions */
        auto GetEntity() const -> Entity { return m_self; }

        /** @name Texture Functions */
        auto GetTexture() const noexcept -> const asset::TextureView& { return m_texture; }
        void SetTexture(const asset::TextureView& texture);

        /** @name ParticleInfo Functions */
        auto GetInfo() const noexcept -> const ParticleInfo& { return m_info; }
        void SetInfo(const ParticleInfo& info);

        /** @brief Emit count number of particles, saturating at maxParticleCount. */
        void Emit(size_t count);

        /** @cond internal */
        static void RegisterSubsystem(graphics::ParticleSubsystem* subsystem)
        {
            s_subsystem = subsystem;
        }
        /** @endcond internal */

    private:
        static inline graphics::ParticleSubsystem* s_subsystem = nullptr;

        Entity m_self;
        asset::TextureView m_texture;
        ParticleInfo m_info;

        void Release() noexcept;
};
} // namespace nc
