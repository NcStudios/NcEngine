#include "LogicModule.h"
#include "ecs/component/Logic.h"
#include "ecs/Registry.h"
#include "ecs/View.h"
#include "module/Job.h"
#include "time/Time.h"

#include "optick/optick.h"

namespace nc::ecs
{
LogicModule::LogicModule(Registry* registry) noexcept
    : m_registry{registry}
{
}

auto LogicModule::BuildWorkload() -> std::vector<Job>
{
    return std::vector<Job>
    {
        Job{ [this]{Run();}, "LogicModule", HookPoint::Logic }
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
} // namespace nc