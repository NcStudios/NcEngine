#pragma once

#include "component/ParticleEmitter.h"
#include "ecs/SoA.h"
#include "graphics/MvpMatrices.h"

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

    // struct GraphicsData
    // {
    //     DirectX::FXMMATRIX viewMatrix;
    //     DirectX::FXMMATRIX projectionMatrix;
    //     graphics::Graphics* graphics;
    // };

    class EmitterState
    {
        public:
            using ParticleSoA = ecs::SoA<Particle, graphics::MvpMatrices>;
            static constexpr size_t ParticlesIndex = 0u;
            static constexpr size_t MvpMatricesIndex = 1u;

            EmitterState(Entity entity, const ParticleInfo& info);

            void Emit(size_t count);
            void Update(float dt);
            const ParticleSoA* GetSoA() const;
            ParticleInfo* GetInfo();
            Entity GetEntity() const;

        private:
            void PeriodicEmission(float dt);
            graphics::MvpMatrices ComputeMvp(const Particle& particle, const Quaternion& camRotation, const Vector3& camForward) const;
            
            ParticleSoA m_soa;
            ParticleInfo m_info;
            //GraphicsData* m_graphicsData;
            Entity m_entity;
            float m_emissionCounter;
    };
}