#include "physics_module_impl.h"
#include "config/Config.h"
#include "engine/modules.h"
#include "graphics/DebugRenderer.h"

namespace
{
    struct bsp_tree_stub
    {
        bsp_tree_stub(nc::Registry* registry)
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

    class physics_module_stub : public nc::physics_module
    {
        public:
            physics_module_stub(nc::Registry* reg) : m_tasks{}, m_bspStub{reg} {}
            void AddJoint(nc::Entity, nc::Entity, const nc::Vector3&, const nc::Vector3&, float = 0.2f, float = 0.0f) override {}
            void RemoveJoint(nc::Entity, nc::Entity) override {}
            void RemoveAllJoints(nc::Entity) override {}
            void RegisterClickable(nc::IClickable*) override {}
            void UnregisterClickable(nc::IClickable*) noexcept override {}
            auto RaycastToClickables(nc::LayerMask = nc::LayerMaskAll) -> nc::IClickable* override { return nullptr;}
            void clear() noexcept override {}
            auto get_tasks() -> nc::TaskGraph& override { return m_tasks; }

        private:
            nc::TaskGraph m_tasks;
            bsp_tree_stub m_bspStub;
    };
}

namespace nc::physics
{
    auto build_physics_module(bool enableModule, Registry* registry) -> std::unique_ptr<physics_module>
    {
        if(enableModule)
        {
            return std::make_unique<physics_module_impl>(registry);
        }
        else
        {
            return std::make_unique<physics_module_stub>(registry);
        }
    }

    physics_module_impl::physics_module_impl(Registry* registry)
        : m_pipeline{registry, config::GetPhysicsSettings().fixedUpdateInterval},
          m_clickableSystem{}
    {
    }

    void physics_module_impl::AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias, float softness)
    {
        m_pipeline.GetJointSystem()->AddJoint(entityA, entityB, anchorA, anchorB, bias, softness);
    }

    void physics_module_impl::RemoveJoint(Entity entityA, Entity entityB)
    {
        m_pipeline.GetJointSystem()->RemoveJoint(entityA, entityB);
    }

    void physics_module_impl::RemoveAllJoints(Entity entity)
    {
        m_pipeline.GetJointSystem()->RemoveAllJoints(entity);
    }

    void physics_module_impl::RegisterClickable(IClickable* clickable)
    {
        m_clickableSystem.RegisterClickable(clickable);
    }
    
    void physics_module_impl::UnregisterClickable(IClickable* clickable) noexcept
    {
        m_clickableSystem.UnregisterClickable(clickable);
    }

    auto physics_module_impl::RaycastToClickables(LayerMask mask) -> IClickable*
    {
        return m_clickableSystem.RaycastToClickables(mask);
    }

    void physics_module_impl::clear() noexcept
    {
        /** @todo make sure these are noexcept */

        m_pipeline.Clear();
        m_clickableSystem.Clear();
    }

    void physics_module_impl::DoPhysicsStep(tf::Executor& taskExecutor)
    {
        #ifdef NC_DEBUG_RENDERING_ENABLED
        graphics::DebugRenderer::ClearPoints();
        graphics::DebugRenderer::ClearPlanes();
        graphics::DebugRenderer::ClearLines();
        #endif

        m_pipeline.Step(taskExecutor);
    }
} // namespace nc::physics