#include "PhysicsSystemImpl.h"
#include "config/Config.h"

namespace nc::physics
{
    PhysicsSystemImpl::PhysicsSystemImpl(Registry* registry, graphics::Graphics* graphics)
        : m_pipeline{registry, config::GetPhysicsSettings().fixedUpdateInterval},
          m_clickableSystem{graphics}
          #ifdef NC_DEBUG_RENDERING
          , m_debugRenderer{graphics}
          #endif
    {
    }

    void PhysicsSystemImpl::AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias, float softness)
    {
        m_pipeline.GetJointSystem()->AddJoint(entityA, entityB, anchorA, anchorB, bias, softness);
    }

    void PhysicsSystemImpl::RemoveJoint(Entity entityA, Entity entityB)
    {
        m_pipeline.GetJointSystem()->RemoveJoint(entityA, entityB);
    }

    void PhysicsSystemImpl::RemoveAllJoints(Entity entity)
    {
        m_pipeline.GetJointSystem()->RemoveAllJoints(entity);
    }

    void PhysicsSystemImpl::RegisterClickable(IClickable* clickable)
    {
        m_clickableSystem.RegisterClickable(clickable);
    }
    
    void PhysicsSystemImpl::UnregisterClickable(IClickable* clickable) noexcept
    {
        m_clickableSystem.UnregisterClickable(clickable);
    }

    auto PhysicsSystemImpl::RaycastToClickables(LayerMask mask) -> IClickable*
    {
        return m_clickableSystem.RaycastToClickables(mask);
    }

    #ifdef NC_DEBUG_RENDERING
    void PhysicsSystemImpl::DebugRender()
    {
        m_debugRenderer.Render();
    }
    #endif

    void PhysicsSystemImpl::ClearState()
    {
        m_pipeline.Clear();
        m_clickableSystem.Clear();
    }

    void PhysicsSystemImpl::DoPhysicsStep(tf::Executor& taskExecutor)
    {
        #ifdef NC_DEBUG_RENDERING
        m_debugRenderer.ClearLines();
        m_debugRenderer.ClearPoints();
        #endif

        m_pipeline.Step(taskExecutor);
    }
} // namespace nc::physics