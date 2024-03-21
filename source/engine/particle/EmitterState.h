#pragma once

#include "ecs/SoA.h"
#include "graphics/ParticleEmitter.h"
#include "math/Random.h"

#include "DirectXMath.h"

#include <vector>

namespace nc
{
    class Transform;
}

namespace nc::particle
{
    struct Particle
    {
        float maxLifetime;
        float currentLifetime;
        Vector3 position;
        Vector3 linearVelocity;
        float rotation;
        float angularVelocity;
        float scale;
    };

    class EmitterState
    {
    public:
        using ParticleSoA = ecs::SoA<Particle, DirectX::XMMATRIX>;
        static constexpr size_t ParticlesIndex = 0u;
        static constexpr size_t ModelMatrixIndex = 1u;

        EmitterState(Entity entity, const graphics::ParticleInfo& info, Random* random);

        void Emit(size_t count);
        void Update(float dt, const DirectX::FXMVECTOR& camRotation, const DirectX::FXMVECTOR& camForward);
        void UpdateInfo(const graphics::ParticleInfo& info);
        const ParticleSoA* GetSoA() const;
        const graphics::ParticleInfo& GetInfo() const;
        Entity GetEntity() const;

        auto GetTexture() const noexcept -> const std::string&
        {
            return m_info.init.particleTexturePath;
        }

    private:
        void PeriodicEmission(float dt);
        auto ComputeMvp(const Particle& particle, const DirectX::FXMVECTOR& camRotation, const DirectX::FXMVECTOR& camForward) const->DirectX::XMMATRIX;

        ParticleSoA m_soa;
        graphics::ParticleInfo m_info;
        Entity m_entity;
        float m_emissionCounter;
        Random* m_random;
        bool m_needsResize = false;
    };
}
