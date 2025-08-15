#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/math/Random.h"

#include "DirectXMath.h"

#include <vector>

namespace nc::graphics
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
    Gradient color;
};

struct ParticleFrameData
{
    DirectX::XMMATRIX matrix{};
    Color color{};
};

class EmitterState
{
    public:
        EmitterState(DirectX::FXMVECTOR position,
                     Entity entity,
                     uint32_t textureIndex,
                     const ParticleInfo& info,
                     Random* random);

        void Emit(DirectX::FXMVECTOR position, size_t count);
        void UpdateInfo(const ParticleInfo& info);
        void UpdateTexture(uint32_t textureIndex);
        void Update(DirectX::FXMVECTOR position,
                    DirectX::FXMVECTOR camRotation,
                    DirectX::FXMVECTOR camForward,
                    float dt);


        auto GetInfo()         const noexcept -> const ParticleInfo&                   { return m_info; }
        auto GetEntity()       const noexcept -> Entity                                { return m_entity; }
        auto GetTextureIndex() const noexcept -> uint32_t                              { return m_textureIndex; }
        auto GetFrameData()    const noexcept -> const std::vector<ParticleFrameData>& { return m_frameData; }
        auto GetLastPosition() const noexcept -> DirectX::FXMVECTOR                    { return m_lastPosition; }

    private:
        void PeriodicEmission(DirectX::FXMVECTOR position, float dt);

        DirectX::XMVECTOR m_lastPosition = DirectX::g_XMZero;
        std::vector<Particle> m_particles;
        std::vector<ParticleFrameData> m_frameData;
        uint32_t m_textureIndex;
        ParticleInfo m_info;
        Entity m_entity;
        Random* m_random;
        float m_emissionCounter = 0.0f;
        bool m_needsResize = false;
};
} // namespace nc::graphics
