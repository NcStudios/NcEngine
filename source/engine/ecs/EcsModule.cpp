#include "EcsModule.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/View.h"
#include "ncengine/ecs/detail/FreeComponentGroup.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/time/Time.h"
#include "ncengine/type/EngineId.h"
#include "ncengine/utility/Log.h"

#include "optick.h"

namespace
{
void UpdateWorldSpaceMatrices(nc::ecs::Ecs world)
{
    using namespace nc;

    struct Place
    {
        Transform* transform;
        std::span<Entity> children;
    };

    auto stack = std::vector<Place>{};
    for (auto e : world.GetAll<Entity>())
    {
        auto& h = world.Get<Hierarchy>(e);
        if (h.parent.Valid())
        {
            continue;
        }

        // can have UpdateWorldMatrix() return dirty state, then split below?

        auto t = &world.Get<Transform>(e);
        t->UpdateWorldMatrix();
        if (h.children.empty())
        {
            continue;
        }

        stack.emplace_back(t, h.children);

        while (!stack.empty())
        {
            auto& children = stack.back().children;
            if (children.empty())
            {
                stack.pop_back();
                continue;
            }

            auto& child = world.Get<Transform>(children.front());
            child.UpdateWorldMatrix(stack.back().transform->TransformationMatrix());

            auto& childH = world.Get<Hierarchy>(children.front());
            if (!childH.children.empty())
            {
                stack.emplace_back(&child, childH.children);
            }

            children = children.subspan(1);
        }


        // auto& h = world.Get<Hierarchy>(e);
        // auto& t = world.Get<Transform>(e);
        // if (h.IsRoot())
        //     t.UpdateWorldMatrix(DirectX::XMMatrixIdentity());
        // else
        // {
        //     auto& parent = world.Get<Transform>(h.parent);
        //     t.UpdateWorldMatrix(parent.TransformationMatrix());
        // }
    }
}
} // anonymous namespace

namespace nc::ecs
{
auto BuildEcsModule(Registry* registry) -> std::unique_ptr<EcsModule>
{
    NC_LOG_TRACE("Creating ECS Module");
    return std::make_unique<EcsModule>(registry);
}

EcsModule::EcsModule(Registry* registry) noexcept
    : Module{NcEcsId},
      m_registry{registry}
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
        [registry = m_registry]
        {
            registry->CommitStagedChanges();
            auto groups = registry->StorageFor<ecs::detail::FreeComponentGroup>();
            for (auto& group : groups->GetComponents())
            {
                group.CommitStagedComponents();
            }

            UpdateWorldSpaceMatrices(registry->GetEcs());
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
} // namespace nc::ecs
