#include "EmitterState.h"
#include "ecs/Registry.h"
#include "math/Random.h"
#include "ncmath/Math.h"

#include <ranges>

namespace
{
using namespace nc;

auto ComputeMvp(const particle::Particle& particle,
                const DirectX::FXMVECTOR& camRotation,
                const DirectX::FXMVECTOR& camForward) -> DirectX::XMMATRIX
{
    const auto rotationRelativeToCamera = DirectX::XMQuaternionRotationAxis(camForward, particle.rotation);
    const auto rotation = DirectX::XMQuaternionMultiply(camRotation, rotationRelativeToCamera);
    return DirectX::XMMatrixScaling(particle.scale, particle.scale, particle.scale) *
           DirectX::XMMatrixRotationQuaternion(rotation) *
           DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadVector3(&particle.position));
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
} // anonymous namespace

namespace nc::particle
{
EmitterState::EmitterState(Entity entity, const graphics::ParticleInfo& info, Random* random)
    : m_info{ info },
      m_entity{ entity },
      m_random{ random }
{
    m_particles.reserve(info.emission.maxParticleCount);
    Emit(m_info.emission.initialEmissionCount);
}

void EmitterState::Emit(size_t count)
{
    m_lastPosition = ActiveRegistry()->Get<Transform>(m_entity)->PositionXM();
    auto parentPosition = Vector3{};
    DirectX::XMStoreVector3(&parentPosition, m_lastPosition);
    const auto particleCount = Min(count, m_particles.capacity() - m_particles.size());
    std::ranges::generate_n(
        std::back_inserter(m_particles),
        particleCount,
        [this, &parentPosition]()
        {
            return CreateParticle(m_info, parentPosition, m_random);
        }
    );
}

void EmitterState::Update(float dt, const DirectX::FXMVECTOR& camRotation, const DirectX::FXMVECTOR& camForward)
{
    if (m_needsResize)
    {
        // We can't just reserve because the capacity is logically important. In the case of shrinking, we'd need
        // to be careful to handle that correctly. Since this functionality is only intended for the editor, we
        // can get away with a simple 'kill everything' approach.
        m_particles.clear();
        m_particles.shrink_to_fit();
        m_particles.reserve(m_info.emission.maxParticleCount);
        m_needsResize = false;
    }

    PeriodicEmission(dt);
    m_matrices.clear();

    if (m_particles.empty())
        return;

    const auto velOverTimeFactor = m_info.kinematic.velocityOverTimeFactor * dt;
    const auto rotOverTimeFactor = m_info.kinematic.rotationOverTimeFactor * dt;
    const auto sclOverTimeFactor = m_info.kinematic.scaleOverTimeFactor * dt;

    for (auto& particle : std::views::reverse(m_particles))
    {
        particle.currentLifetime += dt;
        if (particle.currentLifetime >= particle.maxLifetime)
        {
            particle = m_particles.back();
            m_particles.pop_back();
        }
        else
        {
            ApplyKinematics(&particle, dt, velOverTimeFactor, rotOverTimeFactor, sclOverTimeFactor);
            m_matrices.push_back(::ComputeMvp(particle, camRotation, camForward));
        }
    }
}

void EmitterState::UpdateInfo(const graphics::ParticleInfo& info)
{
    // delay resize so we don't blow up the particle task
    if (info.emission.maxParticleCount != m_info.emission.maxParticleCount)
    {
        m_needsResize = true;
    }

    m_info = info;
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
} // namespace nc::particle
