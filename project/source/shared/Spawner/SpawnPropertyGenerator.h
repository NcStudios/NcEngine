#pragma once
#include "SpawnBehavior.h"
#include <random>

namespace nc::sample
{
    /** Generates values for spawning prefabs based on a SpawnBehavior. */
    class SpawnPropertyGenerator
    {
        public:
            SpawnPropertyGenerator(SpawnBehavior behavior);
            Vector3 RandomVector3();
            Vector3 Position();
            Vector3 Rotation();
            Vector3 Velocity();
            Vector3 RotationAxis();
            float Theta();

        private:
            SpawnBehavior m_behavior;
            std::random_device m_device;
            std::mt19937 m_gen;
            std::uniform_real_distribution<float> m_distribution;
    };

    inline SpawnPropertyGenerator::SpawnPropertyGenerator(SpawnBehavior behavior)
        : m_behavior{behavior},
          m_device{},
          m_gen{m_device()},
          m_distribution{-1.0f, 1.0f}
    {
    }

    inline Vector3 SpawnPropertyGenerator::RandomVector3()
    {
        return Vector3{m_distribution(m_gen), m_distribution(m_gen), m_distribution(m_gen)};
    }

    inline Vector3 SpawnPropertyGenerator::Position()
    {
        return HadamardProduct(RandomVector3(), m_behavior.positionRandomRange) + m_behavior.positionOffset;
    }

    inline Vector3 SpawnPropertyGenerator::Rotation()
    {
        return HadamardProduct(RandomVector3(), m_behavior.rotationRandomRange) + m_behavior.rotationOffset;
    }

    inline Vector3 SpawnPropertyGenerator::Velocity()
    {
        return HadamardProduct(RandomVector3(), m_behavior.velocityRandomRange);
    }

    inline Vector3 SpawnPropertyGenerator::RotationAxis()
    {
        return HadamardProduct(RandomVector3(), m_behavior.rotationAxisRandomRange);
    }

    inline float SpawnPropertyGenerator::Theta()
    {
        return m_distribution(m_gen) * m_behavior.thetaRandomRange;
    }
}