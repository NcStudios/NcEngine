#include "EcsModule.h"
#include "ncengine/Events.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/View.h"
#include "ncengine/ecs/detail/FreeComponentGroup.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/time/Time.h"
#include "ncengine/type/EngineId.h"
#include "ncengine/utility/Log.h"

#include "optick.h"

namespace nc::ecs
{
auto BuildEcsModule(Registry* registry, SystemEvents& events) -> std::unique_ptr<EcsModule>
{
    NC_LOG_TRACE("Creating ECS Module");
    return std::make_unique<EcsModule>(registry, events);
}

EcsModule::EcsModule(Registry* registry, SystemEvents& events) noexcept
    : Module{NcEcsId},
      m_registry{registry},
      m_rebuildStaticConnection{events.rebuildStatics.Connect(
          [this](){ UpdateStaticWorldSpaceMatrices(); }
      )}
{
}

void EcsModule::OnBuildTaskGraph(task::TaskGraph& graph)
{
    NC_LOG_TRACE("Building EcsModule workload");
    graph.Add
    (
        task::ExecutionPhase::Logic,
        "EcsModule",
        [this] { RunFrameLogic(); }
    );

    graph.Add
    (
        task::ExecutionPhase::PreRenderSync,
        "Commit Registry Changes",
        [this]
        {
            m_registry->CommitStagedChanges();
            auto groups = m_registry->StorageFor<ecs::detail::FreeComponentGroup>();
            for (auto& group : groups->GetComponents())
            {
                group.CommitStagedComponents();
            }

            UpdateWorldSpaceMatrices();
        }
    );
}

void EcsModule::RunFrameLogic()
{
    OPTICK_CATEGORY("RunFrameLogic", Optick::Category::GameLogic);
    const float dt = time::DeltaTime();
    for(auto& logic : View<FrameLogic>{m_registry})
    {
        logic.Run(m_registry, dt);
    }
}

void EcsModule::UpdateWorldSpaceMatrices()
{
    auto world = m_registry->GetEcs();

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
    auto world = m_registry->GetEcs();
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
