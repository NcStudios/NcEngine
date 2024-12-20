#pragma once

#include "ParticleRenderState.h"

#include "particle/EmitterState.h"
#include "ncengine/asset/AssetViews.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/math/Random.h"

namespace nc::graphics
{
class ParticleSubsystem
{

    static constexpr int TaskStateUnlocked = 0;
    static constexpr int TaskStateLocked = 1;

    public:
        // todo: explicit?
        ParticleSubsystem(ecs::Ecs world,
                          uint32_t maxParticles);

        // API Facing Functions
        void AddEmitter(ParticleEmitter& emitter);
        void RemoveEmitter(Entity entity);
        void UpdateEmitter(ParticleEmitter& emitter);
        void Emit(Entity entity, size_t count);


        void Update(Camera* mainCamera);
        void CommitPendingChanges();
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
