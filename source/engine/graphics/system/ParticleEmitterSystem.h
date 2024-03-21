#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ecs/Registry.h"
#include "graphics/ParticleEmitter.h"
#include "graphics/Camera.h"
#include "particle/EmitterState.h"
#include "math/Random.h"

#include "graphics/shader_resource/ShaderResourceBus.h"
#include "graphics/shader_resource/StorageBufferHandle.h"

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
        ParticleEmitterSystem(Registry* registry, ShaderResourceBus* shaderResourceBus, std::function<graphics::Camera* ()> getCamera);

        /** Run is able to be run from the JobSystem, but it must finish before
         *  RenderParticles is called. ProcessFrameEvents should be called after rendering to
         *  finalize requests from game logic (additions/deletions). A side effect of this is
         *  particles won't be rendered until the frame after they are created. */
        void Run();
        std::span<const particle::EmitterState> GetParticles() const;
        void ProcessFrameEvents();

        // this may need to be delayed too
        void Emit(Entity entity, size_t count);
        void UpdateInfo(graphics::ParticleEmitter& emitter);

        // ComponentSystem Methods
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
};
} // namespace nc::graphics
