#include "EmitterState.h"
#include "math/Random.h"
#include "ncengine/debug/Profile.h"
#include "ncmath/Math.h"

#include <ranges>

namespace
{
using namespace nc;

auto ComputeMvp(const graphics::Particle& particle,
                const DirectX::FXMVECTOR& camRotation,
                const DirectX::FXMVECTOR& camForward) -> DirectX::XMMATRIX
{
    const auto rotationRelativeToCamera = DirectX::XMQuaternionRotationAxis(camForward, particle.rotation);
    const auto rotation = DirectX::XMQuaternionMultiply(camRotation, rotationRelativeToCamera);
    return DirectX::XMMatrixScaling(particle.scale, particle.scale, particle.scale) *
           DirectX::XMMatrixRotationQuaternion(rotation) *
           DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadVector3(&particle.position));
}

auto CreateParticle(const ParticleInfo& info,
                    const Vector3& positionOffset,
                    Random* random) -> graphics::Particle
{
    const auto& [emission, init, kinematic, color] = info;
    return graphics::Particle
    {
        .maxLifetime = init.lifetime,
        .currentLifetime = 0.0f,
        .position = positionOffset + random->Between(init.positionMin, init.positionMax),
        .linearVelocity = random->Between(kinematic.velocityMin, kinematic.velocityMax),
        .rotation = random->Between(init.rotationMin, init.rotationMax),
        .angularVelocity = random->Between(kinematic.rotationMin, kinematic.rotationMax),
        .scale = random->Between(init.scaleMin, init.scaleMax),
        .color = nc::Gradient{
            info.color.start.Lerp(random->Between(0.0f, 1.0f)),
            info.color.end.Lerp(random->Between(0.0f, 1.0f))
        }
    };
}

void ApplyKinematics(graphics::Particle* particle, float dt, float velOverTimeFactor, float rotOverTimeFactor, float sclOverTimeFactor)
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

auto ApplyCurve(float t, nc::CurveType curve) -> float
{
    switch (curve)
    {
        case CurveType::Linear:    return t;
        case CurveType::EaseIn:    return t * t;
        case CurveType::EaseOut:   return 1.0f - (1.0f - t) * (1.0f - t);
        case CurveType::EaseInOut: return t * t * (3 - 2 * t);
        case CurveType::Spike:     return 1.0f - fabsf(2.0f * t - 1.0f);
        case CurveType::Constant:  return 0.0f;
        default:                   return t;
    }
}
} // anonymous namespace

namespace nc::graphics
{
EmitterState::EmitterState(DirectX::FXMVECTOR position,
                           Entity entity,
                           uint32_t textureIndex,
                           const ParticleInfo& info,
                           Random* random)
    : m_textureIndex{textureIndex},
      m_info{ info },
      m_entity{ entity },
      m_random{ random }
{
    m_particles.reserve(info.emission.maxParticleCount);
    Emit(position, m_info.emission.initialEmissionCount);
}

void EmitterState::Emit(DirectX::FXMVECTOR position, size_t count)
{
    m_lastPosition = position;
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

void EmitterState::Update(DirectX::FXMVECTOR position,
                          DirectX::FXMVECTOR camRotation,
                          DirectX::FXMVECTOR camForward,
                          float dt)
{
    NC_PROFILE_SCOPE("EmitterState::Update", ProfileCategory::VFX);
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

    PeriodicEmission(position, dt);
    m_frameData.clear();

    if (m_particles.empty())
        return;

    const auto velOverTimeFactor = m_info.kinematic.velocityOverTimeFactor * dt;
    const auto rotOverTimeFactor = m_info.kinematic.rotationOverTimeFactor * dt;
    const auto sclOverTimeFactor = m_info.kinematic.scaleOverTimeFactor * dt;

    auto end = m_particles.size();
    for (auto& particle : std::views::reverse(m_particles))
    {
        particle.currentLifetime += dt;
        if (particle.currentLifetime >= particle.maxLifetime)
        {
            particle = m_particles.at(--end);
        }
        else
        {
            ApplyKinematics(&particle, dt, velOverTimeFactor, rotOverTimeFactor, sclOverTimeFactor);
            const auto t = particle.currentLifetime / particle.maxLifetime;
            const auto colorFactor = ApplyCurve(t, m_info.color.colorCurve);
            const auto alphaFactor = ApplyCurve(t, m_info.color.alphaCurve);
            m_frameData.emplace_back(
                ::ComputeMvp(particle, camRotation, camForward),
                particle.color.Lerp(colorFactor, alphaFactor)
            );
        }
    }

    if (end != m_particles.size())
    {
        m_particles.erase(m_particles.begin() + end, m_particles.end());
    }
}

void EmitterState::UpdateInfo(const ParticleInfo& info)
{
    // delay resize so we don't blow up the particle task
    if (info.emission.maxParticleCount != m_info.emission.maxParticleCount)
    {
        m_needsResize = true;
    }

    m_info = info;
}

void EmitterState::UpdateTexture(uint32_t textureIndex)
{
    m_textureIndex = textureIndex;
}

void EmitterState::PeriodicEmission(DirectX::FXMVECTOR position, float dt)
{
    if (m_info.emission.periodicEmissionFrequency > 0.0f)
    {
        m_emissionCounter += dt;
        if (m_emissionCounter > m_info.emission.periodicEmissionFrequency)
        {
            m_emissionCounter = 0.0f;
            Emit(position, m_info.emission.periodicEmissionCount);
        }
    }
}
} // namespace nc::graphics
