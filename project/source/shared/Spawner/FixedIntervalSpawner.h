#pragma once
#include "Spawner.h"

namespace nc::sample
{
    /** A Spawner that operates on a regular interval. */
    class FixedIntervalSpawner : public Spawner
    {
        public:
            FixedIntervalSpawner(Entity entity,
                                 registry_type* registry,
                                 prefab::Resource resource,
                                 SpawnBehavior behavior,
                                 float spawnRate,
                                 Spawner::SpawnExtension extension = nullptr);
            
            void FrameUpdate(float dt) override;
            void SetSpawnRate(float rate);

        private:
            float m_spawnRate;
            float m_lastSpawnTime = 0.0f;
    };

    inline FixedIntervalSpawner::FixedIntervalSpawner(Entity entity,
                                                      registry_type* registry,
                                                      prefab::Resource resource,
                                                      SpawnBehavior behavior,
                                                      float spawnRate,
                                                      Spawner::SpawnExtension extension)
        : Spawner{entity, registry, resource, behavior, extension},
          m_spawnRate{spawnRate}
    {
    }

    inline void FixedIntervalSpawner::FrameUpdate(float dt)
    {
        m_lastSpawnTime += dt;
        if(m_lastSpawnTime > m_spawnRate)
        {
            m_lastSpawnTime = 0.0f;
            //Spawner::Spawn();

            //temp
            Spawner::Spawn(10u);
        }
    }

    inline void FixedIntervalSpawner::SetSpawnRate(float rate)
    {
        m_spawnRate = math::Clamp(rate, 0.0001f, 10.0f);
    }
}