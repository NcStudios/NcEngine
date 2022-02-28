#include "PhysicsSystemImpl.h"
#include "config/Config.h"
#include "engine/engine_context.h"
#include "graphics/DebugRenderer.h"

namespace
{
    struct physics_system_stub : public nc::context_stub
    {
        physics_system_stub(nc::Registry* registry)
        {
            registry->RegisterOnAddCallback<nc::ConcaveCollider>
            (
                [this](nc::ConcaveCollider& collider){ this->OnAdd(collider); }
            );
            
            registry->RegisterOnRemoveCallback<nc::ConcaveCollider>
            (
                [this](nc::Entity entity) { this->OnRemove(entity); }
            );
        }

        void OnAdd(nc::ConcaveCollider&) {}
        void OnRemove(nc::Entity) {}
    };
}

namespace nc::physics
{
    void attach_physics_system(engine_context* context, Registry* registry, bool enableModule)
    {
        if(enableModule)
        {
            context->physicsSystem = std::make_unique<PhysicsSystemImpl>(registry);
        }
        else
        {
            context->physicsSystem = nullptr;
            context->stubs.push_back(std::make_unique<physics_system_stub>(registry));
        }
    }

    PhysicsSystemImpl::PhysicsSystemImpl(Registry* registry)
        : m_pipeline{registry, config::GetPhysicsSettings().fixedUpdateInterval},
          m_clickableSystem{}
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

    void PhysicsSystemImpl::ClearState()
    {
        m_pipeline.Clear();
        m_clickableSystem.Clear();
    }

    void PhysicsSystemImpl::DoPhysicsStep(tf::Executor& taskExecutor)
    {
        #ifdef NC_DEBUG_RENDERING_ENABLED
        graphics::DebugRenderer::ClearPoints();
        graphics::DebugRenderer::ClearPlanes();
        graphics::DebugRenderer::ClearLines();
        #endif

        m_pipeline.Step(taskExecutor);
    }
} // namespace nc::physics