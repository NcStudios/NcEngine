#include "EmitterState.h"

namespace
{
    using namespace nc;

    particle::Particle CreateParticle(const ParticleInfo& info)
    {
        Vector3 position = info.particlePosition * (rand() % 5);
        float randRot = rand() % 3;
        Quaternion rotation = Quaternion::FromEulerAngles(randRot, randRot, randRot);
        Vector3 scale = Vector3::Splat((rand() % 3) + 1);

        return particle::Particle
        {
            .maxLifetime = info.particleLifetime,
            .currentLifetime = 0.0f,
            .position = position,
            .rotation = rotation,
            .scale = scale
        };
    }
}

namespace nc::particle
{
    EmitterState::EmitterState(EntityHandle handle, const ParticleInfo& info)
        : Component{handle},
          m_info{info},
          m_soa{info.maxParticleCount}
    {
        Emit(m_info.initialEmissionCount);
    }

    void EmitterState::Emit(size_t count)
    {
        // if full ...

        for(unsigned i = 0; i < count; ++i)
        {
            auto particle = CreateParticle(m_info);

            m_soa.Add
            (
                particle.maxLifetime,
                particle.currentLifetime,
                particle.position,
                particle.rotation,
                particle.scale
            );
        
        }
    }

    void EmitterState::Update(float dt)
    {
        std::vector<unsigned> toRemove;
        auto [index, maxLifetimes, currentLifetimes] = m_soa.View<MaxLifetimesIndex, CurrentLifetimesIndex>();

        while(index.Valid())
        {
            currentLifetimes[index] += dt;
            if(currentLifetimes[index] >= maxLifetimes[index])
                toRemove.push_back(index);

            ++index;
        }

        for(auto i : toRemove)
            m_soa.RemoveAtIndex(i);

    }

    auto EmitterState::GetSoA() -> ecs::SoA<float, float, Vector3, Quaternion, Vector3>*
    {
        return &m_soa;
    }
} // namespace nc::particle