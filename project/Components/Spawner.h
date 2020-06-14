#pragma once

#include "NCE.h"
#include "graphics/Model.h"

#include <vector>
#include <random>

class Spawner : public nc::Component
{
    public:
        Spawner();

        void OnInitialize() override;
        void FrameUpdate(float dt) override;

    private:
        std::vector<nc::EntityHandle> m_entities;

        std::mt19937 m_rng;
        std::uniform_real_distribution<float> m_angDist;
        std::uniform_real_distribution<float> m_posDist;
        std::uniform_real_distribution<float> m_sclDist;
        std::uniform_real_distribution<float> m_clrDist;

        nc::graphics::Mesh m_monkeyMesh;
        nc::graphics::Mesh m_ncMesh;

        float m_timeSinceSpawn;
        float m_timeSinceDespawn;
        const float m_spawnRate = 0.5f;
        const float m_despawnRate = 3.0f;
        const unsigned int m_initialCount = 10u;
};