#include "EmitterState.h"
#include "ecs/Registry.h"
#include "MainCamera.h"
#include "math/Random.h"

namespace
{
    using namespace nc;

    // DirectX::XMMATRIX ComposeMatrix(float scale, const Quaternion& r, const Vector3& pos)
    // {
    //     return DirectX::XMMatrixScaling(scale, scale, scale) * 
    //            DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(r.x, r.y, r.z, r.w)) *
    //            DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
    // }

    // particle::Particle CreateParticle(const ParticleInfo& info, const Vector3& positionOffset)
    // {
    //     const auto& [emission, init, kinematic] = info;

    //     return particle::Particle
    //     {
    //         .maxLifetime = init.lifetime,
    //         .currentLifetime = 0.0f,
    //         .position = positionOffset + random::Vec3(init.position, init.positionRange),
    //         .linearVelocity = random::Vec3(kinematic.velocity, kinematic.velocityRange),
    //         .rotation = random::Float(init.rotation, init.rotationRange), //.rotation + g_random.Float() * init.rotationRange,
    //         .angularVelocity = random::Float(kinematic.rotation, kinematic.rotationRange), //kinematic.rotation + g_random.Float() * kinematic.rotationRange,
    //         .scale = random::Float(init.scale, init.scaleRange) //g_random.Float() * init.scaleRange + init.scale
    //     };
    // }

    // void ApplyKinematics(particle::Particle* particle, float dt, float velOverTimeFactor, float rotOverTimeFactor, float sclOverTimeFactor)
    // {
    //     auto& vel = particle->linearVelocity;
    //     vel = vel + vel * velOverTimeFactor;
    //     particle->position = particle->position + vel * dt;

    //     auto& angVel = particle->angularVelocity;
    //     angVel += angVel * rotOverTimeFactor;
    //     particle->rotation += angVel * dt;

    //     auto& scale = particle->scale;
    //     scale = math::Clamp(scale + scale * sclOverTimeFactor * dt, 0.000001f, 5000.0f); // defaults?
    // }
}

namespace nc::particle
{
    EmitterState::EmitterState(Entity entity, const ParticleInfo& info)
        : m_soa{info.emission.maxParticleCount},
          m_info{info},
          m_entity{entity},
          m_emissionCounter{0.0f}
    {
        Emit(m_info.emission.initialEmissionCount);
    }

    void EmitterState::Emit(size_t)
    {
        //auto parentPosition = ActiveRegistry()->Get<Transform>(m_entity)->Position();
        //auto particleCount = math::Min(count, m_soa.GetRemainingSpace());
        //for(size_t i = 0; i < particleCount; ++i)
        //{
            //m_soa.Add(CreateParticle(m_info, parentPosition), {});
        //}
    }

    void EmitterState::Update(float)
    {
        // PeriodicEmission(dt);

        // std::vector<unsigned> toRemove; // linear allocator?
        // const auto velOverTimeFactor = m_info.kinematic.velocityOverTimeFactor * dt;
        // const auto rotOverTimeFactor = m_info.kinematic.rotationOverTimeFactor * dt;
        // const auto sclOverTimeFactor = m_info.kinematic.scaleOverTimeFactor * dt;
        // auto* camera = GetMainCameraTransform();
        // auto camRotation = camera->Rotation();
        // auto camForward = camera->Forward();
        // auto [index, particles, matrices] = m_soa.View<ParticlesIndex, MvpMatricesIndex>();

        // for(; index.Valid(); ++index)
        // {
        //     auto& particle = particles[index];
        //     particle.currentLifetime += dt;
        //     if(particle.currentLifetime >= particle.maxLifetime)
        //         toRemove.push_back(index);
        //     else
        //     {
        //         ApplyKinematics(&particle, dt, velOverTimeFactor, rotOverTimeFactor, sclOverTimeFactor);
        //         matrices[index] = ComputeMvp(particle, camRotation, camForward);
        //     }
        // }

        // for(auto i : toRemove)
        //     m_soa.RemoveAtIndex(i);
    }

    ParticleInfo* EmitterState::GetInfo()
    {
        return &m_info;
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

    graphics::MvpMatrices EmitterState::ComputeMvp(const Particle&, const Quaternion&, const Vector3&) const
    {
        return graphics::MvpMatrices{};

        // const auto modelView = ComposeMatrix
        // (
        //     particle.scale,
        //     Multiply(camRotation, Quaternion::FromAxisAngle(camForward, particle.rotation)),
        //     particle.position
        // ) * m_graphicsData->viewMatrix;


        // return graphics::MvpMatrices
        // {
        //     // @todo: Replace MvpMatrices with Normal + VP matrices.
        //     ComposeMatrix(particle.scale, Multiply(camRotation, Quaternion::FromAxisAngle(camForward, particle.rotation)), particle.position),
        //     DirectX::XMMatrixTranspose(modelView * m_graphicsData->projectionMatrix)
        // };
    }
} // namespace nc::particle