#include "EmitterState.h"
#include "Ecs.h"
#include "MainCamera.h"

#include <random>

namespace
{
    using namespace nc;

    DirectX::XMMATRIX ComposeMatrix(float scale, const Quaternion& r, const Vector3& pos)
    {
        return DirectX::XMMatrixScaling(scale, scale, scale) * 
               DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(r.x, r.y, r.z, r.w)) *
               DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
    }

    /** @todo Not quite sure where this should go - it may be worth it to extend this
     *  a bit and expose it through the api. Random generation helpers would be
     *  useful for game logic (sample scene spawner already duplicates this code). */
    class Random
    {
        public:
            Random()
                : m_device{},
                  m_gen{m_device()},
                  m_distribution{-1.0f, 1.0f}
            {
            }

        float Float()
        {
            return m_distribution(m_gen);
        }

        Vector3 Vec3(Vector3 offset, Vector3 range)
        {
            return offset + HadamardProduct(Vector3{Float(), Float(), Float()}, range);
        }

        private:
            std::random_device m_device;
            std::mt19937 m_gen;
            std::uniform_real_distribution<float> m_distribution;
    };

    Random g_random;

    particle::Particle CreateParticle(const ParticleInfo& info, const Vector3& positionOffset)
    {
        const auto& [emission, init, kinematic] = info;

        return particle::Particle
        {
            .maxLifetime = init.lifetime,
            .currentLifetime = 0.0f,
            .position = positionOffset + g_random.Vec3(init.position, init.positionRange),
            .linearVelocity = g_random.Vec3(kinematic.velocity, kinematic.velocityRange),
            .rotation = init.rotation + g_random.Float() * init.rotationRange,
            .angularVelocity = kinematic.rotation + g_random.Float() * kinematic.rotationRange,
            .scale = g_random.Float() * init.scaleRange + init.scale
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
        scale = math::Clamp(scale + scale * sclOverTimeFactor * dt, 0.000001f, 5000.0f); // defaults?
    }
}

namespace nc::particle
{
    EmitterState::EmitterState(EntityHandle handle, const ParticleInfo& info, GraphicsData* graphicsData)
        : m_soa{info.emission.maxParticleCount},
          m_info{info},
          m_graphicsData{graphicsData},
          m_transform{GetComponent<Transform>(handle)},
          m_handle{handle},
          m_emissionCounter{0.0f}
    {
        Emit(m_info.emission.initialEmissionCount);
    }

    void EmitterState::Emit(size_t count)
    {
        auto parentPosition = m_transform->GetPosition();
        auto particleCount = math::Min(count, m_soa.GetRemainingSpace());
        for(size_t i = 0; i < particleCount; ++i)
        {
            m_soa.Add(CreateParticle(m_info, parentPosition), {});
        }
    }

    void EmitterState::Update(float dt)
    {
        PeriodicEmission(dt);

        std::vector<unsigned> toRemove; // linear allocator?
        const auto velOverTimeFactor = m_info.kinematic.velocityOverTimeFactor * dt;
        const auto rotOverTimeFactor = m_info.kinematic.rotationOverTimeFactor * dt;
        const auto sclOverTimeFactor = m_info.kinematic.scaleOverTimeFactor * dt;
        auto* camera = camera::GetMainCameraTransform();
        auto camRotation = camera->GetRotation();
        auto camForward = camera->Forward();
        auto [index, particles, matrices] = m_soa.View<ParticlesIndex, MvpMatricesIndex>();

        for(; index.Valid(); ++index)
        {
            auto& particle = particles[index];
            particle.currentLifetime += dt;
            if(particle.currentLifetime >= particle.maxLifetime)
                toRemove.push_back(index);
            else
            {
                ApplyKinematics(&particle, dt, velOverTimeFactor, rotOverTimeFactor, sclOverTimeFactor);
                matrices[index] = ComputeMvp(particle, camRotation, camForward);
            }
        }

        for(auto i : toRemove)
            m_soa.RemoveAtIndex(i);
    }

    auto EmitterState::GetSoA() const -> const ParticleSoA*
    {
        return &m_soa;
    }

    EntityHandle EmitterState::GetHandle() const
    {
        return m_handle;
    }

    void EmitterState::PeriodicEmission(float dt)
    {
        if(m_info.emission.periodicEmissionFrequency > 0.0f)
        {
            m_emissionCounter += dt;
            if(m_emissionCounter > m_info.emission.periodicEmissionFrequency)
            {
                m_emissionCounter = 0.0f;
                Emit(m_info.emission.periodicEmissionCount);
            }
        }
    }

    graphics::MvpMatrices EmitterState::ComputeMvp(const Particle& particle, const Quaternion& camRotation, const Vector3& camForward) const
    {
        const auto modelView = ComposeMatrix
        (
            particle.scale,
            Multiply(camRotation, Quaternion::FromAxisAngle(camForward, particle.rotation)),
            particle.position
        ) * m_graphicsData->viewMatrix;

        return graphics::MvpMatrices
        {
            DirectX::XMMatrixTranspose(modelView),
            DirectX::XMMatrixTranspose(modelView * m_graphicsData->projectionMatrix)
        };
    }
} // namespace nc::particle