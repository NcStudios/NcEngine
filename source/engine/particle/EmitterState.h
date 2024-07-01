#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/math/Random.h"

#include "DirectXMath.h"

#include <vector>

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

struct Doohickey
{
    DirectX::XMMATRIX matrix;
    Vector4 color;
};

class EmitterState
{
public:
    EmitterState(ecs::ExplicitEcs<Transform> transforms, Entity entity, const graphics::ParticleInfo& info, Random* random);

    void Emit(size_t count);
    void Update(float dt, const DirectX::FXMVECTOR& camRotation, const DirectX::FXMVECTOR& camForward);
    void UpdateInfo(const graphics::ParticleInfo& info);

    auto GetInfo() const noexcept -> const graphics::ParticleInfo& { return m_info; }
    auto GetEntity() const noexcept -> Entity { return m_entity; }
    auto GetTexture() const noexcept -> const std::string& { return m_info.init.particleTexturePath; }
    auto GetMatrices() const noexcept -> const std::vector<Doohickey>& { return m_matrices; }
    auto GetLastPosition() const noexcept -> DirectX::FXMVECTOR { return m_lastPosition; }

private:
    void PeriodicEmission(float dt);

    DirectX::XMVECTOR m_lastPosition = DirectX::g_XMZero;
    std::vector<Particle> m_particles;
    std::vector<Doohickey> m_matrices;
    std::vector<Vector4> m_colors;
    graphics::ParticleInfo m_info;
    ecs::ExplicitEcs<Transform> m_transforms;
    Entity m_entity;
    Random* m_random;
    float m_emissionCounter = 0.0f;
    bool m_needsResize = false;
};
} // namespace nc::particle
