#pragma once

#include "ecs/component/Logic.h"
#include "ecs/Registry.h"
#include "ecs/View.h"
#include "module/Module.h"
#include "module/Job.h"

#include "optick/optick.h"

namespace nc
{
    class LogicModule : public Module
    {
        public:
            LogicModule(Registry* registry, float* dt)
                : m_registry{registry},
                  m_dt{dt}
            {
            }

            auto BuildWorkload() -> std::vector<Job> override
            {
                return std::vector<Job>
                {
                    Job{ [this]{Run();}, "LogicModule", HookPoint::Logic }
                };
            }

            void Clear() noexcept override {}

            void Run()
            {
                OPTICK_CATEGORY("Logic", Optick::Category::GameLogic);
                const float dt = *m_dt;
                for(auto& logic : View<FrameLogic>{m_registry})
                    logic.Run(m_registry, dt);
            }

        private:
            Registry* m_registry;
            float* m_dt;
    };
}