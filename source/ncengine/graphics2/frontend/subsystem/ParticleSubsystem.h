#pragma once

#include "ParticleRenderState.h"

#include "particle/EmitterState.h"
#include "ncengine/asset/AssetViews.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/math/Random.h"



#include <atomic>

template<class T>
class AtomicState
{
    public:
        auto Exchange(T value) -> T
        {
            return m_value.exchange(value);
        }

        auto Load() -> T
        {
             return m_value.load();
        }


    private:
        std::atomic_int m_value;
        static_assert(std::atomic_int::is_always_lock_free);
};

#define NC_DEFINE_STATE_VALIDATOR(type, name, initialValue) std::atomic<type> name = initialValue;

#define NC_ASSERT_STATE(flag, expected) \
    NC_ASSERT( \
        flag.load() == expected, \
        fmt::format("AtomicState mismtach") \
    );

#define NC_TRANSITION_STATE(flag, expected, newState) NC_ASSERT(flag.exchange(newState) == expected, "AtomicState mismatch")

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
        void AddEmitter(graphics::ParticleEmitter& emitter);
        void RemoveEmitter(Entity entity);
        void UpdateEmitter(graphics::ParticleEmitter& emitter);
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
        std::vector<ParticleData2> m_particleDataHostBuffer; // structured buffer?...
        uint32_t m_maxParticles;

        NC_DEFINE_STATE_VALIDATOR(int, m_taskState, TaskStateUnlocked);

        void SortEmitters(DirectX::FXMVECTOR cameraPosition);
};
} // namespace nc::graphics
