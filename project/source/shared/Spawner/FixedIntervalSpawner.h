#pragma once
#include "Spawner.h"

namespace nc::sample
{
    /** A Spawner that operates on a regular interval. */
    class FixedIntervalSpawner : public Spawner
    {
        public:
            FixedIntervalSpawner(EntityHandle handle, prefab::Resource resource, SpawnBehavior behavior, float spawnRate, Spawner::SpawnExtension extension = nullptr);
            void FrameUpdate(float dt) override;

        private:
            float m_spawnRate;
            float m_lastSpawnTime = 0.0f;
    };

    inline FixedIntervalSpawner::FixedIntervalSpawner(EntityHandle handle, prefab::Resource resource, SpawnBehavior behavior, float spawnRate, Spawner::SpawnExtension extension)
        : Spawner{handle, resource, behavior, extension},
          m_spawnRate{spawnRate}
    {
    }

    inline void FixedIntervalSpawner::FrameUpdate(float dt)
    {
        m_lastSpawnTime += dt;
        if(m_lastSpawnTime > m_spawnRate)
        {
            m_lastSpawnTime = 0.0f;
            Spawner::Spawn();
        }
    }
}