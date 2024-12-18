#pragma once

#include "graphics2/ShaderTypes.h"

#include "particle/EmitterState.h"
#include "ncengine/asset/AssetViews.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/math/Random.h"

namespace nc::graphics
{
struct ParticleData2
{
    DirectX::XMMATRIX model;
    uint32_t textureIndex;
};

struct ParticleRenderState
{
    BufferUpdateInfo<ParticleData2> particleData;
    asset::MeshView mesh;
};

class ParticleSubsystem
{
    public:
        // todo: explicit?
        ParticleSubsystem(ecs::Ecs world,
                          uint32_t maxParticles);

        // API Facing Functions
        void AddEmitter(graphics::ParticleEmitter& emitter);
        void RemoveEmitter(Entity entity);
        void UpdateEmitter(graphics::ParticleEmitter& emitter);
        void Emit(Entity entity, size_t count);


        void Update();
        void CommitPendingChanges();
        auto BuildState() -> ParticleRenderState;
        void Clear() noexcept;

    private:
        std::vector<particle::EmitterState> m_emitterStates;
        std::vector<particle::EmitterState> m_toAdd;
        std::vector<Entity> m_toRemove;
        Random m_random;
        std::function<graphics::Camera* ()> m_getCamera;
        ecs::Ecs m_world;
        std::vector<ParticleData2> m_particleDataHostBuffer; // structured buffer?...
        uint32_t m_maxParticles;

        void SortEmitters(DirectX::FXMVECTOR cameraPosition);
};
} // namespace nc::graphics
