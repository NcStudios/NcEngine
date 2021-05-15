#pragma once
#include "SpawnBehavior.h"
#include "random/Random.h"

namespace nc::sample
{
    /** Generates values for spawning prefabs based on a SpawnBehavior. */
    class SpawnPropertyGenerator
    {
        public:
            SpawnPropertyGenerator(SpawnBehavior behavior);
            Vector3 Position();
            Vector3 Rotation();
            Vector3 Velocity();
            Vector3 RotationAxis();
            float Theta();

        private:
            SpawnBehavior m_behavior;
    };

    inline SpawnPropertyGenerator::SpawnPropertyGenerator(SpawnBehavior behavior)
        : m_behavior{behavior}
    {
    }

    inline Vector3 SpawnPropertyGenerator::Position()
    {
        return random::Vec3(m_behavior.positionOffset, m_behavior.positionRandomRange);
    }

    inline Vector3 SpawnPropertyGenerator::Rotation()
    {
        return random::Vec3(m_behavior.rotationOffset, m_behavior.rotationRandomRange);
    }

    inline Vector3 SpawnPropertyGenerator::Velocity()
    {
        return random::Vec3(Vector3::Zero(), m_behavior.velocityRandomRange);
    }

    inline Vector3 SpawnPropertyGenerator::RotationAxis()
    {
        return random::Vec3(Vector3::Zero(), m_behavior.rotationAxisRandomRange);
    }

    inline float SpawnPropertyGenerator::Theta()
    {
        return random::Float(0.0f, m_behavior.thetaRandomRange);
    }
}