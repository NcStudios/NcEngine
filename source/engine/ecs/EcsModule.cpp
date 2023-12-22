#include "EcsModule.h"
#include "ecs/Logic.h"
#include "ecs/Registry.h"
#include "ecs/View.h"
#include "ecs/detail/FreeComponentGroup.h"
#include "task/TaskGraph.h"
#include "time/Time.h"
#include "utility/Log.h"

#include "optick.h"

namespace nc::ecs
{
auto BuildEcsModule(Registry* registry) -> std::unique_ptr<EcsModule>
{
    NC_LOG_TRACE("Creating ECS Module");
    return std::make_unique<EcsModule>(registry);
}

EcsModule::EcsModule(Registry* registry) noexcept
    : m_registry{registry}
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
