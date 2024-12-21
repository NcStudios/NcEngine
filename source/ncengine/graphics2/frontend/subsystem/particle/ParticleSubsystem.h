#pragma once

#include "EmitterState.h"
#include "ParticleRenderState.h"

#include "ncengine/asset/AssetViews.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/math/Random.h"

namespace nc::graphics
{
class ParticleSubsystem
{
    public:
        explicit ParticleSubsystem(ecs::Ecs world,
                                   uint32_t maxParticles);

        // API Facing Functions
        void AddEmitter(ParticleEmitter& emitter);
        void RemoveEmitter(Entity entity);
        void UpdateEmitterInfo(Entity entity, const ParticleInfo& info);
        void UpdateEmitterTexture(Entity entity, uint32_t textureIndex);
        void Emit(Entity entity, size_t count);

        // Update Graph Tasks
        void Update(Camera* mainCamera);
        void CommitPendingChanges();

        // GraphcsFrontend Functions
        auto BuildState() -> ParticleRenderState;
        void Clear() noexcept;

    private:
        std::vector<particle::EmitterState> m_emitterStates;
        std::vector<particle::EmitterState> m_toAdd;
        std::vector<Entity> m_toRemove;
        Random m_random;
        ecs::Ecs m_world;
        std::vector<ParticleData> m_particleDataHostBuffer;
        uint32_t m_maxParticles;

        void SortEmitters(DirectX::FXMVECTOR cameraPosition);
};
} // namespace nc::graphics
