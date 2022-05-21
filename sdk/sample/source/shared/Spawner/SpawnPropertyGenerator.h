#pragma once
#include "SpawnBehavior.h"
#include "math/Random.h"

namespace nc::sample
{
    /** Generates values for spawning prefabs based on a SpawnBehavior. */
    class SpawnPropertyGenerator
    {
        public:
            SpawnPropertyGenerator(SpawnBehavior behavior, Random* random);
            Vector3 Position();
            Vector3 Rotation();
            Vector3 Velocity();
            Vector3 RotationAxis();
            float Theta();

        private:
            SpawnBehavior m_behavior;
            Random* m_random;
    };

    inline SpawnPropertyGenerator::SpawnPropertyGenerator(SpawnBehavior behavior, Random* random)
        : m_behavior{behavior},
          m_random{random}
    {
    }

    inline Vector3 SpawnPropertyGenerator::Position()
    {
        return m_random->Between(m_behavior.minPosition, m_behavior.maxPosition);
    }

    inline Vector3 SpawnPropertyGenerator::Rotation()
    {
        return m_random->Between(m_behavior.minRotation, m_behavior.maxRotation);
    }

    inline Vector3 SpawnPropertyGenerator::Velocity()
    {
        return m_random->Between(m_behavior.minVelocity, m_behavior.maxVelocity);
    }

    inline Vector3 SpawnPropertyGenerator::RotationAxis()
    {
        return m_random->Between(Vector3::Zero(), m_behavior.rotationAxis);
    }

    inline float SpawnPropertyGenerator::Theta()
    {
        return m_random->Between(0.0f, m_behavior.rotationTheta);
    }
}