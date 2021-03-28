#include "PhysicsSystem.h"

namespace nc::physics
{
    /* Physics System */
    PhysicsSystem::PhysicsSystem(const PhysicsSystemInfo& info)
        : m_collisionSystem{info.maxDynamicColliders,
                            info.maxStaticColliders,
                            info.octreeDensityThreshold,
                            info.octreeMinimumExtent,
                            info.worldspaceExtent,
                            info.jobSystem},
          m_clickableSystem{info.graphics}
    {
    }

    void PhysicsSystem::ClearState()
    {
        m_clickableSystem.Clear();
        m_collisionSystem.ClearState();
    }

    void PhysicsSystem::DoPhysicsStep()
    {
        m_collisionSystem.DoCollisionStep();
    }

    #ifdef NC_EDITOR_ENABLED
    void PhysicsSystem::UpdateWidgets(graphics::FrameManager* frameManager)
    {
        m_collisionSystem.UpdateWidgets(frameManager);
    }
    #endif
} // namespace nc::physics