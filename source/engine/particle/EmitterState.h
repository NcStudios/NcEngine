#pragma once

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

class EmitterState
{
public:
    EmitterState(Entity entity, const graphics::ParticleInfo& info, Random* random);

    void Emit(size_t count);
    void Update(float dt, const DirectX::FXMVECTOR& camRotation, const DirectX::FXMVECTOR& camForward);
    void UpdateInfo(const graphics::ParticleInfo& info);

    auto GetInfo() const noexcept -> const graphics::ParticleInfo& { return m_info; }
    auto GetEntity() const noexcept -> Entity { return m_entity; }
    auto GetTexture() const noexcept -> const std::string& { return m_info.init.particleTexturePath; }
    auto GetMatrices() const noexcept -> const std::vector<DirectX::XMMATRIX>& { return m_matrices; }
    auto GetLastPosition() const noexcept -> DirectX::FXMVECTOR { return m_lastPosition; }

private:
    void PeriodicEmission(float dt);

    std::vector<Particle> m_particles;
    std::vector<DirectX::XMMATRIX> m_matrices;
    graphics::ParticleInfo m_info;
    DirectX::XMVECTOR m_lastPosition = DirectX::g_XMZero;
    Entity m_entity;
    Random* m_random;
    float m_emissionCounter = 0.0f;
    bool m_needsResize = false;
};
} // namespace nc::particle
