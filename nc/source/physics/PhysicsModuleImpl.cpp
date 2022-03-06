#include "PhysicsModuleImpl.h"
#include "config/Config.h"
#include "engine/modules.h"
#include "graphics/DebugRenderer.h"

namespace
{
    struct BspTreeStub
    {
        BspTreeStub(nc::Registry* registry)
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

    class PhysicsModuleStub : public nc::PhysicsModule
    {
        public:
            PhysicsModuleStub(nc::Registry* reg) : m_tasks{}, m_bspStub{reg} {}
            void AddJoint(nc::Entity, nc::Entity, const nc::Vector3&, const nc::Vector3&, float = 0.2f, float = 0.0f) override {}
            void RemoveJoint(nc::Entity, nc::Entity) override {}
            void RemoveAllJoints(nc::Entity) override {}
            void RegisterClickable(nc::IClickable*) override {}
            void UnregisterClickable(nc::IClickable*) noexcept override {}
            auto RaycastToClickables(nc::LayerMask = nc::LayerMaskAll) -> nc::IClickable* override { return nullptr;}
            void Clear() noexcept override {}
            auto GetTasks() -> nc::TaskGraph& override { return m_tasks; }

        private:
            nc::TaskGraph m_tasks;
            BspTreeStub m_bspStub;
    };
}

namespace nc::physics
{
    auto BuildPhysicsModule(bool enableModule, Registry* registry) -> std::unique_ptr<PhysicsModule>
    {
        if(enableModule)
        {
            return std::make_unique<PhysicsModuleImpl>(registry);
        }
        else
        {
            return std::make_unique<PhysicsModuleStub>(registry);
        }
    }

    PhysicsModuleImpl::PhysicsModuleImpl(Registry* registry)
        : m_pipeline{registry, config::GetPhysicsSettings().fixedUpdateInterval},
          m_clickableSystem{}
    {
    }

    void PhysicsModuleImpl::AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias, float softness)
    {
        m_pipeline.GetJointSystem()->AddJoint(entityA, entityB, anchorA, anchorB, bias, softness);
    }

    void PhysicsModuleImpl::RemoveJoint(Entity entityA, Entity entityB)
    {
        m_pipeline.GetJointSystem()->RemoveJoint(entityA, entityB);
    }

    void PhysicsModuleImpl::RemoveAllJoints(Entity entity)
    {
        m_pipeline.GetJointSystem()->RemoveAllJoints(entity);
    }

    void PhysicsModuleImpl::RegisterClickable(IClickable* clickable)
    {
        m_clickableSystem.RegisterClickable(clickable);
    }
    
    void PhysicsModuleImpl::UnregisterClickable(IClickable* clickable) noexcept
    {
        m_clickableSystem.UnregisterClickable(clickable);
    }

    auto PhysicsModuleImpl::RaycastToClickables(LayerMask mask) -> IClickable*
    {
        return m_clickableSystem.RaycastToClickables(mask);
    }

    void PhysicsModuleImpl::Clear() noexcept
    {
        /** @todo make sure these are noexcept */

        m_pipeline.Clear();
        m_clickableSystem.Clear();
    }
} // namespace nc::physics