#pragma once

#include "graphics/shader_resource/ShaderResourceBus.h"
#include "graphics/shader_resource/StorageBufferHandle.h"
#include "particle/EmitterState.h"
#include "ncengine/asset/AssetViews.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/math/Random.h"

namespace nc::graphics
{
struct ParticleData
{
    DirectX::XMMATRIX model;
    uint32_t textureIndex;
};

struct ParticleState
{
    MeshView mesh;
    uint32_t count;
};

class ParticleEmitterSystem
{
    public:
        ParticleEmitterSystem(Registry* registry,
                              ShaderResourceBus* shaderResourceBus,
                              std::function<graphics::Camera* ()> getCamera,
                              unsigned maxParticles);

        void UpdateParticles();
        void ProcessFrameEvents();
        void Emit(Entity entity, size_t count);
        void UpdateInfo(graphics::ParticleEmitter& emitter);

        void Add(graphics::ParticleEmitter& emitter);
        void Remove(Entity entity);
        void Clear();

        auto Execute(uint32_t frameIndex) -> ParticleState;

    private:
        std::vector<particle::EmitterState> m_emitterStates;
        std::vector<particle::EmitterState> m_toAdd;
        std::vector<Entity> m_toRemove;
        Random m_random;
        std::function<graphics::Camera* ()> m_getCamera;
        Registry* m_registry;
        Connection<graphics::ParticleEmitter&> m_onAddConnection;
        Connection<Entity> m_onRemoveConnection;
        std::vector<ParticleData> m_particleDataHostBuffer;
        StorageBufferHandle m_particleDataDeviceBuffer;
        unsigned m_maxParticles;

        void SortEmitters(DirectX::FXMVECTOR cameraPosition);
};
} // namespace nc::graphics
