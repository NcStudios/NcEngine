#include "LogicModule.h"
#include "ecs/Logic.h"
#include "ecs/Registry.h"
#include "ecs/View.h"
#include "task/Job.h"
#include "time/Time.h"

#include "optick/optick.h"

namespace nc::ecs
{
LogicModule::LogicModule(Registry* registry) noexcept
    : m_registry{registry}
{
}

auto LogicModule::BuildWorkload() -> std::vector<task::Job>
{
    return std::vector<task::Job>
    {
        task::Job{ [this]{Run();}, "LogicModule", task::ExecutionPhase::Logic }
    };
}

void LogicModule::Run()
{
    OPTICK_CATEGORY("Logic", Optick::Category::GameLogic);
    const float dt = time::DeltaTime();
    for(auto& logic : View<FrameLogic>{m_registry})
    {
        logic.Run(m_registry, dt);
    }
}
} // namespace nc::ecs
