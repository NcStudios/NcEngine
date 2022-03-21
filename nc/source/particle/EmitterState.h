#pragma once

#include "ecs/component/ParticleEmitter.h"
#include "ecs/SoA.h"
#include "math/Random.h"

#include "directx/Inc/DirectXMath.h"

#include <vector>

namespace nc
{
    class Transform;
    namespace graphics { class Graphics; }
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

        EmitterState(Entity entity, const ParticleInfo& info, Random* random);

        void Emit(size_t count);
        void Update(float dt, const Quaternion& camRotation, const Vector3& camForward);
        const ParticleSoA* GetSoA() const;
        const ParticleInfo& GetInfo() const;
        Entity GetEntity() const;

    private:
        void PeriodicEmission(float dt);
        auto ComputeMvp(const Particle& particle, const Quaternion& camRotation, const Vector3& camForward) const->DirectX::XMMATRIX;

        ParticleSoA m_soa;
        ParticleInfo m_info;
        Entity m_entity;
        float m_emissionCounter;
        Random* m_random;
    };
}