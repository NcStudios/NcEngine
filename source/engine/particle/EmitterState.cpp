#include "EmitterState.h"
#include "ecs/Registry.h"
#include "math/Random.h"
#include "ncmath/Math.h"

namespace
{
    using namespace nc;

    DirectX::XMMATRIX ComposeMatrix(float scale, const Quaternion& r, const Vector3& pos)
    {
        return DirectX::XMMatrixScaling(scale, scale, scale) *
            DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(r.x, r.y, r.z, r.w)) *
            DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
    }

    particle::Particle CreateParticle(const graphics::ParticleInfo& info, const Vector3& positionOffset, Random* random)
    {
        const auto& [emission, init, kinematic] = info;


        return particle::Particle
        {
            .maxLifetime = init.lifetime,
            .currentLifetime = 0.0f,
            .position = positionOffset + random->Between(init.positionMin, init.positionMax),
            .linearVelocity = random->Between(kinematic.velocityMin, kinematic.velocityMax),
            .rotation = random->Between(init.rotationMin, init.rotationMax),
            .angularVelocity = random->Between(kinematic.rotationMin, kinematic.rotationMax),
            .scale = random->Between(init.scaleMin, init.scaleMax)
        };
    }

    void ApplyKinematics(particle::Particle* particle, float dt, float velOverTimeFactor, float rotOverTimeFactor, float sclOverTimeFactor)
    {
        auto& vel = particle->linearVelocity;
        vel = vel + vel * velOverTimeFactor;
        particle->position = particle->position + vel * dt;

        auto& angVel = particle->angularVelocity;
        angVel += angVel * rotOverTimeFactor;
        particle->rotation += angVel * dt;

        auto& scale = particle->scale;
        scale = Clamp(scale + scale * sclOverTimeFactor * dt, 0.000001f, 5000.0f); // defaults?
    }
}

namespace nc::particle
{
    EmitterState::EmitterState(Entity entity, const graphics::ParticleInfo& info, Random* random)
        : m_soa{ info.emission.maxParticleCount },
        m_info{ info },
        m_entity{ entity },
        m_emissionCounter{ 0.0f },
        m_random{ random }
    {
        Emit(m_info.emission.initialEmissionCount);
    }

    void EmitterState::UpdateInfo(const graphics::ParticleInfo& info)
    {
        // disabling resizing of underlying soa, fine in editor/rendering code, but UB if called from game logic
        // if (info.emission.maxParticleCount != m_soa.GetSize())
        if (info.emission.maxParticleCount != m_info.emission.maxParticleCount)
        {
            throw NcError{"Resizing of particle system is disabled due to potential UB"};
            // m_soa = ParticleSoA{info.emission.maxParticleCount};
        }

        m_info = info;
    }

    void EmitterState::Emit(size_t count)
    {
        auto parentPosition = ActiveRegistry()->Get<Transform>(m_entity)->Position();
        auto particleCount = Min(count, m_soa.GetRemainingSpace());
        for (size_t i = 0; i < particleCount; ++i)
        {
            m_soa.Add(CreateParticle(m_info, parentPosition, m_random), {});
        }
    }

    void EmitterState::Update(float dt, const Quaternion& camRotation, const Vector3& camForward)
    {
        PeriodicEmission(dt);

        std::vector<unsigned> toRemove; // linear allocator?
        const auto velOverTimeFactor = m_info.kinematic.velocityOverTimeFactor * dt;
        const auto rotOverTimeFactor = m_info.kinematic.rotationOverTimeFactor * dt;
        const auto sclOverTimeFactor = m_info.kinematic.scaleOverTimeFactor * dt;
        auto [index, particles, matrices] = m_soa.View<ParticlesIndex, ModelMatrixIndex>();

        for (; index.Valid(); ++index)
        {
            auto& particle = particles[index];
            particle.currentLifetime += dt;
            if (particle.currentLifetime >= particle.maxLifetime)
                toRemove.push_back(static_cast<uint32_t>(index));
            else
            {
                ApplyKinematics(&particle, dt, velOverTimeFactor, rotOverTimeFactor, sclOverTimeFactor);
                matrices[index] = ComputeMvp(particle, camRotation, camForward);
            }
        }

        for (auto i : toRemove)
            m_soa.RemoveAtIndex(i);
    }

    const graphics::ParticleInfo& EmitterState::GetInfo() const
    {
        return m_info;
    }

    auto EmitterState::GetSoA() const -> const ParticleSoA*
    {
        return &m_soa;
    }

    Entity EmitterState::GetEntity() const
    {
        return m_entity;
    }

    void EmitterState::PeriodicEmission(float dt)
    {
        if (m_info.emission.periodicEmissionFrequency > 0.0f)
        {
            m_emissionCounter += dt;
            if (m_emissionCounter > m_info.emission.periodicEmissionFrequency)
            {
                m_emissionCounter = 0.0f;
                Emit(m_info.emission.periodicEmissionCount);
            }
        }
    }

    auto EmitterState::ComputeMvp(const Particle& particle, const Quaternion& camRotation, const Vector3& camForward) const -> DirectX::XMMATRIX
    {
        return ComposeMatrix
        (
            particle.scale,
            Multiply(camRotation, Quaternion::FromAxisAngle(camForward, particle.rotation)),
            particle.position
        );
    }
} // namespace nc::particle
