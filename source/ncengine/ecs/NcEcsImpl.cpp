#include "NcEcsImpl.h"
#include "ncengine/debug/Profile.h"
#include "ncengine/ecs/ComponentRegistry.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/detail/FreeComponentGroup.h"
#include "ncengine/ecs/FrameLogic.h"
#include "ncengine/Events.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/time/Time.h"
#include "ncengine/utility/Log.h"

namespace nc::ecs
{
auto BuildEcsModule(ComponentRegistry& registry, SystemEvents& events) -> std::unique_ptr<NcEcs>
{
    NC_LOG_TRACE("Creating ECS Module");
    return std::make_unique<EcsModule>(registry, events);
}

EcsModule::EcsModule(ComponentRegistry& registry, SystemEvents& events) noexcept
    : m_registry{&registry},
      m_rebuildStaticConnection{events.rebuildStatics.Connect(
          [this](){ UpdateStaticWorldSpaceMatrices(); }
      )}
{
}

void EcsModule::OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks&)
{
    NC_LOG_TRACE("Building EcsModule Tasks");
    update.Add
    (
        update_task_id::FrameLogicUpdate,
        "FrameLogicUpdate",
        [this] { RunFrameLogic(); }
    );

    update.Add
    (
        update_task_id::CommitStagedChanges,
        "CommitStagedChanges",
        [this]
        {
            m_registry->CommitPendingChanges();
            auto groups = m_registry->GetPool<ecs::detail::FreeComponentGroup>().GetComponents();
            for (auto& group : groups)
            {
                group.CommitStagedComponents();
            }

            UpdateWorldSpaceMatrices();
        },
        {
            update_task_id::AudioSourceUpdate,
            update_task_id::ParticleEmitterUpdate,
            update_task_id::PhysicsPipeline
        }
    );
}

void EcsModule::RunFrameLogic()
{
    NC_PROFILE_TASK("RunFrameLogic", ProfileCategory::GameLogic);
    const float dt = time::DeltaTime();
    auto world = ecs::Ecs{*m_registry};
    for(auto& logic : m_registry->GetPool<FrameLogic>().GetComponents())
    {
        logic.Run(world, dt);
    }
}

void EcsModule::UpdateWorldSpaceMatrices()
{
    NC_PROFILE_TASK("UpdateWorldSpaceMatrices", ProfileCategory::GameLogic);
    auto world = Ecs{*m_registry};

#if 1 // normal update using graph traversal
    struct ParentInfo
    {
        Transform* transform;
        std::span<Entity> children;
    };

    auto stack = std::vector<ParentInfo>{};
    for (auto entity : world.GetAll<Entity>())
    {
        if (entity.IsStatic())
            continue;

        auto& hierarchy = world.Get<Hierarchy>(entity);
        if (hierarchy.parent.Valid()) // process root nodes first
            continue;

        auto& transform = world.Get<Transform>(entity);
        auto dirty = transform.IsDirty();
        if (dirty)
            transform.UpdateWorldMatrix();

        if (hierarchy.children.empty())
            continue;

        stack.emplace_back(&transform, hierarchy.children);
        while (!stack.empty())
        {
            auto& children = stack.back().children;
            if (children.empty())
            {
                stack.pop_back();
                continue;
            }

            if (!children.front().IsStatic())
            {
                auto& child = world.Get<Transform>(children.front());
                dirty = dirty || child.IsDirty();
                if (dirty)
                    child.UpdateWorldMatrix(stack.back().transform->TransformationMatrix());

                auto& childHierarchy = world.Get<Hierarchy>(children.front());
                if (!childHierarchy.children.empty())
                    stack.emplace_back(&child, childHierarchy.children);
            }

            children = children.subspan(1);
        }
    }
#else // debug update
    for (auto entity : world.GetAll<Entity>())
    {
        auto& hierarchy = world.Get<Hierarchy>(entity);
        auto& transform = world.Get<Transform>(entity);
        if (!hierarchy.parent.Valid())
            transform.UpdateWorldMatrix();
        else
        {
            auto& parentTransform = world.Get<Transform>(hierarchy.parent);
            transform.UpdateWorldMatrix(parentTransform.TransformationMatrix());
        }
    }
#endif
}

void EcsModule::UpdateStaticWorldSpaceMatrices()
{
    auto world = Ecs{*m_registry};
    for (auto entity : world.GetAll<Entity>())
    {
        if (!entity.IsStatic())
            continue;

        auto& hierarchy = world.Get<Hierarchy>(entity);
        auto& transform = world.Get<Transform>(entity);
        if (!hierarchy.parent.Valid())
            transform.UpdateWorldMatrix();
        else
        {
            auto& parentTransform = world.Get<Transform>(hierarchy.parent);
            transform.UpdateWorldMatrix(parentTransform.TransformationMatrix());
        }
    }
}
} // namespace nc::ecs
